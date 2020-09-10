// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/CharacterBase.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine.h"

#include "CharacterController.h"

#include "Attacks/Attack.h"
#include "Attacks/AbilityAttack.h"
#include "Attacks/AttackingComponent.h"
#include "Attacks/BasicAttack.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(40.0f, 100.0f);
	
	BaseTurnRate = 45.0f;
	SprintConsumptionDelay = 2.0f;
	StatsRecoveryDelay = 3.0f;
	MaxJumpRecoveryTime = 1.5f;
	EyesSocketName = FName("Eyes_Position");

	bChangedPosition = false;
	bIsRunning = false;
	bFinishedJump = true;
	bUseStaminaWhenSprinting = false;
	bRecoverStats = true;
	bCanMove = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->UpdatedComponent = GetCapsuleComponent();

	// Don't use the controller rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Create the Spring Arm Component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create the third person camera
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("3rd Person Camera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	AttackingComponent = CreateDefaultSubobject<UAttackingComponent>(TEXT("Attacking Component"));
	AttackingComponent->SetAutoActivate(true);
	AttackingComponent->OwnerCharacter = this;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Initialise attributes that are set from blueprints
	CurrentStats = MaxStats;
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Reduce stamina while sprinting
	if (bUseStaminaWhenSprinting)
	{
		CurrentStats.Stamina = FMath::Max(CurrentStats.Stamina - StatsCost.SprintStaminaCost * DeltaTime, 0.0f);
		if (CurrentStats.Stamina == 0.0f)
		{
			OnSprintStop();
		}
	}

	// Recover HP and stamina only if the player is not attacking (Do this only when the stats are not full)
	if (bRecoverStats)
	{
		CurrentStats.HP = FMath::Clamp(CurrentStats.HP + CurrentStats.HPRecovery * DeltaTime, 0.0f, MaxStats.HP);
		CurrentStats.FP = FMath::Clamp(CurrentStats.FP + CurrentStats.FPRecovery * DeltaTime, 0.0f, MaxStats.FP);
		CurrentStats.Stamina = FMath::Clamp(CurrentStats.Stamina + CurrentStats.StaminaRecovery * DeltaTime, 0.0f, MaxStats.Stamina);
	}
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* Action Mappings */
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterBase::OnJumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterBase::OnJumpStop);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACharacterBase::OnSprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACharacterBase::OnSprintStop);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ACharacterBase::OnPrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ACharacterBase::OnSecondaryAttack);

	// Start ability targeting
	PlayerInputComponent->BindAction("AbilityOne", IE_Pressed, this, &ACharacterBase::OnAbilityOneTarget);
	PlayerInputComponent->BindAction("AbilityTwo", IE_Pressed, this, &ACharacterBase::OnAbilityTwoTarget);
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &ACharacterBase::OnUltimateAbilityTarget);

	// Perform ability attack
	PlayerInputComponent->BindAction("AbilityOne", IE_Released, this, &ACharacterBase::OnAbilityOneAttack);
	PlayerInputComponent->BindAction("AbilityTwo", IE_Released, this, &ACharacterBase::OnAbilityTwoAttack);
	PlayerInputComponent->BindAction("Ultimate", IE_Released, this, &ACharacterBase::OnUltimateAbilityAttack);

	/* Axis Mappings */
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);
}

// ************************* Input Delegates *********************
void ACharacterBase::OnJumpStart()
{
	// If character is attacking
	if (AttackingComponent->IsAttacking() || AttackingComponent->IsTargeting())
	{
		// If cannot interrupt the attack, return
		if (!AttackingComponent->InterruptAttack())
			return;
	}
	else if (CurrentStats.Stamina == 0.0f || bFinishedJump == false)
	{
		// Cannot jump if player has no stamina
		return;
	}

	// Reduce stamina
	CurrentStats.Stamina = FMath::Max(CurrentStats.Stamina - StatsCost.JumpStaminaCost, 0.0f);
	bRecoverStats = false;
	bFinishedJump = false;

	Jump();
}

void ACharacterBase::OnJumpStop()
{
	StopJumping();
}

void ACharacterBase::OnSprintStart()
{
	// If character is attacking
	if (AttackingComponent->IsAttacking())
	{
		// If cannot interrupt the attack, return
		if (!AttackingComponent->InterruptAttack())
			return;
	}

	bIsRunning = true;
	bRecoverStats = false;

	// Setup a delay in stamina consumption if the stamina bar is full
	if (CurrentStats.Stamina == MaxStats.Stamina)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_SprintDelay, [&]() { bUseStaminaWhenSprinting = true; }, SprintConsumptionDelay, false);
	}
	else
	{
		bUseStaminaWhenSprinting = true;
	}
}

void ACharacterBase::OnSprintStop()
{
	bIsRunning = false;
	bUseStaminaWhenSprinting = false;

	// Setup a delay in stats recovery after sprint stops
	if (bRecoverStats == false)
	{
		SetupStatsRecoveryDelay();
	}
}

void ACharacterBase::MoveForward(float Value)
{
	if (Controller != NULL && Value != 0.0f && bCanMove)
	{
		bChangedPosition = true;

		const FRotator Rotation = FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if (Controller != NULL && Value != 0.0f && bCanMove)
	{
		bChangedPosition = true;

		const FRotator Rotation = FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::TurnRate(float Value)
{
	AddControllerYawInput(GetWorld()->GetDeltaSeconds() * Value * BaseTurnRate);
}

void ACharacterBase::OnPrimaryAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Primary Attack", true, 2 * FVector2D::UnitVector);
	
	if (!CanStartAttack(EAttackType::Primary))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "All conditions for Primary attack not fulfilled.");
		return;
	}

	bRecoverStats = !AttackingComponent->OnAttack(EAttackType::Primary);
}

void ACharacterBase::OnSecondaryAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Secondary Attack", true, 2 * FVector2D::UnitVector);

	if (!CanStartAttack(EAttackType::Secondary))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "All conditions for Secondary attack not fulfilled.");
		return;
	}

	bRecoverStats = !AttackingComponent->OnAttack(EAttackType::Secondary);
}

void ACharacterBase::OnAbilityOneTarget()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Ability One Target", true, 2 * FVector2D::UnitVector);
	
	if (!CanStartAttack(EAttackType::AbilityOne))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "All conditions for Ability 1 attack not fulfilled.");
		return;
	}

	if (AttackingComponent->OnTarget(EAttackType::AbilityOne) == true)
	{
		bRecoverStats = false;
		OnStartTargeting(EAttackType::AbilityOne);
	}
}

void ACharacterBase::OnAbilityOneAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Ability One Attack", true, 2 * FVector2D::UnitVector);

	AttackingComponent->OnAttack(EAttackType::AbilityOne);
}

void ACharacterBase::OnAbilityTwoTarget()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Ability Two Target", true, 2 * FVector2D::UnitVector);
	
	if (!CanStartAttack(EAttackType::AbilityTwo))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "All conditions for Ability 2 attack not fulfilled.");
		return;
	}

	if (AttackingComponent->OnTarget(EAttackType::AbilityTwo) == true)
	{
		bRecoverStats = false;
		OnStartTargeting(EAttackType::AbilityTwo);
	}
}

void ACharacterBase::OnAbilityTwoAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Ability Two Attack", true, 2 * FVector2D::UnitVector);
	
	AttackingComponent->OnAttack(EAttackType::AbilityTwo);
}

void ACharacterBase::OnUltimateAbilityTarget()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Ultimate Target", true, 2 * FVector2D::UnitVector);
	
	if (!CanStartAttack(EAttackType::Ultimate))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "All conditions for Ultimate ability attack not fulfilled.");
		return;
	}
	
	if (AttackingComponent->OnTarget(EAttackType::Ultimate) == true)
	{
		bRecoverStats = false;
		OnStartTargeting(EAttackType::Ultimate);
	}
}

void ACharacterBase::OnUltimateAbilityAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Ultimate Attack", true, 2 * FVector2D::UnitVector);
	
	AttackingComponent->OnAttack(EAttackType::Ultimate);
}

// ************************* Input Delegates ********************* ~END

bool ACharacterBase::CanStartAttack(const EAttackType NewAttackType) const
{
	bool CanStart = true;

	// Has the required stats
	CanStart = CanStart && HasRequiredStats(AttackingComponent->GetAttackStatistics(NewAttackType));

	// Is the character in air
	CanStart = CanStart && !GetCharacterMovement()->IsFalling();

	// If the character is sprinting
	if (bIsRunning)
	{
		// Ok to do this here
		const_cast<ACharacterBase*>(this)->OnSprintStop();
	}

	return CanStart;
}

bool ACharacterBase::HasRequiredStats(const FAttackStats& AttackStats) const
{
	// TODO: Think about the stamina requirement
	return (CurrentStats.Stamina > 0.0f /* >= AttacksStats.StaminaCost */) && (CurrentStats.FP >= AttackStats.FPCost);
}

void ACharacterBase::SetupStatsRecoveryDelay()
{
	GetWorldTimerManager().SetTimer(TimerHandle_RecoverStatsDelay, [&]() { bRecoverStats = true; }, StatsRecoveryDelay, false);
}

void ACharacterBase::OnAttackExecuted(const EAttackType Attack, const FAttackStats& AttackStats)
{
	CurrentStats.Stamina = FMath::Max(CurrentStats.Stamina - AttackStats.StaminaCost, 0.0f);

	// This should always be positive. FP Requirement must be complete, unlike stamina
	CurrentStats.FP = FMath::Max(CurrentStats.FP - AttackStats.StaminaCost, 0.0f);
}

void ACharacterBase::OnAttackInterrupted(const EAttackType Attack)
{

}

void ACharacterBase::OnStartTargeting(const EAttackType Ability)
{

}

void ACharacterBase::PostAttackEnd(const EAttackType Attack)
{
	// Set the stats to recover after a certain amount of delay
	SetupStatsRecoveryDelay();
}

void ACharacterBase::OnAbilityTimerEnd(const EAttackType Attack)
{

}

void ACharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	GetWorldTimerManager().SetTimer(TimerHandle_JumpRecoveryFinished, this, &ACharacterBase::OnJumpRecoveryFinished, MaxJumpRecoveryTime, false);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Landed on ground");
}

bool ACharacterBase::IsTargetingAbilityOne() const
{
	UAttack* AbilityOne = AttackingComponent->GetAbilityOne();
	if (AbilityOne != nullptr)
	{
		return AbilityOne->IsTargeting();
	}

	return false;
}

bool ACharacterBase::IsTargetingAbilityTwo() const
{
	UAttack* AbilityTwo = AttackingComponent->GetAbilityTwo();
	if (AbilityTwo != nullptr)
	{
		return AbilityTwo->IsTargeting();
	}

	return false;
}

bool ACharacterBase::IsTargetingUltimate() const
{
	UAttack* Ultimate = AttackingComponent->GetUltimate();
	if (Ultimate != nullptr)
	{
		return Ultimate->IsTargeting();
	}

	return false;
}

void ACharacterBase::OnJumpRecoveryFinished()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_JumpRecoveryFinished);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Finished Jump Recovery");
	bFinishedJump = true;

	// Also recover the stats
	bRecoverStats = true;
}
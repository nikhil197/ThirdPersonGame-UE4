// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/Character/Serath/SerathAbilityTwo.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/DecalActor.h"
#include "TimerManager.h"

#include "Attacks/AttackingComponent.h"
#include "Characters/SerathPlayerCharacter.h"
#include "CharacterController.h"

USerathAbilityTwo::USerathAbilityTwo()
{
	Name = FName("Ascend");

	TargetingDistance = 1500.0f;
	TargetingHeight = 100.0f;

	FRiseInterpSpeed = 15.0f;

	bRising = false;
}

void USerathAbilityTwo::Init(UWorld* InWorld, class UAttackingComponent* InOwnerComponent)
{
	Super::Init(InWorld, InOwnerComponent);

	TargetDecal = World->SpawnActor<ADecalActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	TargetDecal->SetDecalMaterial(TargetDecalMaterial);		// Okay to set here because this will be called from the attacking component BeginPlay()
	TargetDecal->SetActorHiddenInGame(true);
	TargetDecal->SetActorScale3D(2 * FVector(1.0f));

	RiseInterpSpeed = RiseAnimation != nullptr ? TargetingHeight / RiseAnimation->GetPlayLength() : 0.0f;
}

bool USerathAbilityTwo::OnTarget()
{
	ACharacterBase* Character = OwnerComponent->GetOwnerCharacter();
	Character->SetCanMove(false);
	Character->SetFinishedJump(false);
	Character->GetCharacterMovement()->GravityScale = 0.0f;
	Character->GetCharacterMovement()->StopMovementImmediately();

	InAirLocation = Character->GetActorLocation() + FVector(0.0f, 0.0f, TargetingHeight);

	float Duration = Character->PlayAnimMontage(RiseAnimation);
	if (Duration > 0.0f)
	{
		bRising = true;
		World->GetTimerManager().SetTimer(TimerHandle_OnReachedSetHeight, this, &USerathAbilityTwo::OnReachedSetHeight, Duration, false);
	}
	else
	{
		Character->TeleportTo(InAirLocation, Character->GetActorRotation());
		OnReachedSetHeight();
	}

	return Super::OnTarget();
}

bool USerathAbilityTwo::StopTarget()
{
	// Hide the target decal
	TargetDecal->SetActorHiddenInGame(true);

	ACharacterBase* Character = OwnerComponent->GetOwnerCharacter();
	//Character->SetCanMove(true);
	//Character->SetFinishedJump(true);
	Character->GetCharacterMovement()->GravityScale = 2.0f;

	if (bRising)
	{
		World->GetTimerManager().ClearTimer(TimerHandle_OnReachedSetHeight);
		bRising = false;
		Character->StopAnimMontage(RiseAnimation);		// Stop the rise animation if still playing
	}

	return Super::StopTarget();
}

bool USerathAbilityTwo::PreAttack()
{
	// If the character is still rising in the air, postpone the attack
	if (bRising)
	{
		bContinueToAttack = true;
		return true;
	}

	return Super::PreAttack();
}

void USerathAbilityTwo::ExecuteAttack()
{
	Super::ExecuteAttack();

	AttackLocation = TargetDecal->GetActorLocation();
	AttackInterpSpeed = AttackAnimation != nullptr ? (AttackLocation - OwnerComponent->GetOwnerCharacter()->GetActorLocation()).Size() / AttackAnimation->GetPlayLength() : 0.0f; 

	ACharacterBase* Character = OwnerComponent->GetOwnerCharacter();
	FVector Direction = (AttackLocation - Character->GetActorLocation()).GetSafeNormal();
	FRotator Rotation = Direction.Rotation();
	Rotation.Pitch = 0.0f;
	Character->SetActorRotation(Rotation);
}

void USerathAbilityTwo::PostAttack()
{
	Super::PostAttack();

	ACharacterBase* Character = OwnerComponent->GetOwnerCharacter();
	Character->SetCanMove(true);
	Character->SetFinishedJump(true);
	Character->GetCharacterMovement()->GravityScale = 1.0f;
}

void USerathAbilityTwo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ACharacterBase* Character = OwnerComponent->GetOwnerCharacter();
	if (CurrentState == EAttackState::Targeting)
	{
		if (bRising == true)
		{
			float FrameRateMultiplier = FMath::Max(DeltaTime, MaxFrameRateSample);
			Character->SetActorLocation(FMath::VInterpTo(Character->GetActorLocation(), InAirLocation, 1.0f, 0.1f));
		}
		else
		{
			ACharacterController* CharacterController = Cast<ACharacterController>(Character->GetController());
			if (CharacterController != nullptr)
			{
				// check if we need to move the decal if the controller rotation changed
				if (CharacterController->bChangedRotation == true)
				{
					// Update the target transform
					UpdateDecalTransform(DeltaTime);

					CharacterController->bChangedRotation = false;
				}
			}
		}
	}
	else if (CurrentState == EAttackState::Progress && Character->GetActorLocation() != AttackLocation)
	{
		float FrameRateMultiplier = FMath::Max(DeltaTime, MaxFrameRateSample);
		Character->SetActorLocation(FMath::VInterpTo(Character->GetActorLocation(), AttackLocation, FrameRateMultiplier, RiseInterpSpeed * FrameRateMultiplier));
	}
}

void USerathAbilityTwo::UpdateDecalTransform(float DeltaTime, bool bStartedTargeting)
{
	ASerathPlayerCharacter* Character = Cast<ASerathPlayerCharacter>(OwnerComponent->GetOwnerCharacter());
	if (Character != nullptr)
	{
		// Get Actor Rotation
		USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
		FVector StartLocation = CharacterMesh->GetSocketLocation(Character->EyesSocketName);
		FQuat SocketRotation = CharacterMesh->GetSocketQuaternion(Character->EyesSocketName);
		FVector Direction = SocketRotation.GetRightVector();
		FVector EndLocation = StartLocation + Direction * TargetingDistance;

		float InterpSpeed = 3.0f;

		// Run the line trace
		FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(AbilityTwoTargetTrace), true, Character);
		FHitResult Result;
		if (World->LineTraceSingleByChannel(Result, StartLocation, EndLocation, ECollisionChannel::ECC_Camera, TraceParams))
		{
			// Target found with in the range
			if (bStartedTargeting == false)
			{
				// Interpolate only if already targeting
				FVector InterpLoc = FMath::VInterpTo(TargetDecal->GetActorLocation(), Result.Location, DeltaTime, InterpSpeed);
				FRotator InterpRotation = FMath::RInterpTo(TargetDecal->GetActorRotation(), Result.Normal.Rotation(), DeltaTime, InterpSpeed);
				TargetDecal->SetActorLocationAndRotation(InterpLoc, InterpRotation);
			}
			else
			{
				// Snap to the new location if started targeting
				TargetDecal->SetActorLocationAndRotation(Result.Location, Result.Normal.Rotation());
			}
		}
		else
		{
			// If the target is not within range, set the location to the edge of the range\
			// Get a location on the edge of the range
			float Distance = FMath::Sqrt((TargetingDistance * TargetingDistance) - (TargetingHeight * TargetingHeight));
			FVector Location = StartLocation + Direction * Distance;
			TargetDecal->SetActorLocationAndRotation(Location, Character->GetActorUpVector().Rotation());
		}
	}
}

void USerathAbilityTwo::OnReachedSetHeight()
{
	bRising = false;

	TargetDecal->SetActorHiddenInGame(false);
	UpdateDecalTransform(0.0f, true);

	if (bContinueToAttack)
	{
		PreAttack();
		bContinueToAttack = false;
	}
}
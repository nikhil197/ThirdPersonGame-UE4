// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/AttackingComponent.h"

#include "Attacks/Attack.h"
#include "Attacks/BasicAttack.h"
#include "Attacks/AbilityAttack.h"

#include "Characters/CharacterBase.h"
#include "CharacterController.h"

// Sets default values for this component's properties
UAttackingComponent::UAttackingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Set defaults
	bIsAttacking = false;
	bIsTargeting = false;
	CurrentAttack = nullptr;
	OwnerCharacter = nullptr;
}

// Called when the game starts
void UAttackingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	// Create Individual attack objects
	UWorld* World = GetWorld();

	if (PrimaryAttackClass != nullptr)
	{
		PrimaryAttack = NewObject<UAttack>(World, PrimaryAttackClass, FName("Primary Attack"));
		PrimaryAttack->Init(World, this);
	}
	
	if (SecondaryAttackClass != nullptr)
	{
		SecondaryAttack = NewObject<UAttack>(this, SecondaryAttackClass, FName("Secondary Attack"));
		SecondaryAttack->Init(World, this);
	}

	if (AbilityOneClass != nullptr)
	{
		AbilityOne = NewObject<UAttack>(this, AbilityOneClass, FName("Ability One Attack"));
		AbilityOne->Init(World, this);
	}

	if (AbilityTwoClass != nullptr)
	{
		AbilityTwo = NewObject<UAttack>(this, AbilityTwoClass, FName("Ability Two Attack"));
		AbilityTwo->Init(World, this);
	}

	if (UltimateClass != nullptr)
	{
		Ultimate = NewObject<UAttack>(this, UltimateClass, FName("Ultimate Ability Attack"));
		Ultimate->Init(World, this);
	}
}


// Called every frame
void UAttackingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	// Update the target
	if (AbilityOne != nullptr)
	{
		AbilityOne->Tick(DeltaTime);
	}

	if (AbilityTwo != nullptr)
	{
		AbilityTwo->Tick(DeltaTime);
	}

	if (Ultimate != nullptr)
	{
		Ultimate->Tick(DeltaTime);
	}
}

bool UAttackingComponent::OnTarget(const EAttackType AttackType)
{
	// Return false if an attack is in progress. There is no combo setup in targeting and targeting is only for ability attacks
	if (bIsAttacking || AttackType == EAttackType::Primary || AttackType == EAttackType::Secondary)
		return false;

	if (CanTarget(AttackType) == false)
	{
		return false;
	}

	bool Success = false;
	switch (AttackType)
	{
		case EAttackType::AbilityOne:
		{
			if (AbilityOne != nullptr)
			{
				CurrentAttack = AbilityOne;
				Success = AbilityOne->OnTarget();
			}
			
			break;
		}
		case EAttackType::AbilityTwo:
		{
			if (AbilityTwo != nullptr)
			{
				CurrentAttack = AbilityTwo;
				Success = AbilityTwo->OnTarget();
			}

			break;
		}
		case EAttackType::Ultimate:
		{
			if (Ultimate != nullptr)
			{
				CurrentAttack = Ultimate;
				Success = Ultimate->OnTarget();
			}

			break;
		}
	}

	bIsTargeting = Success;
	bIsAttacking = false;
	CurrentAttack = Success ? CurrentAttack : nullptr;

	return true;
}

bool UAttackingComponent::OnAttack(const EAttackType AttackType)
{
	// Reset the targeting state
	bIsTargeting = false;

	if (CanStartAttack(AttackType) == false)
		return false;

	// Attack should not be interrupted if the same attack is in progress (To setup the combos)
	if (CurrentAttack != nullptr && !CurrentAttack->IsTargeting() && !CurrentAttack->CanPerformCombos())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Interrupting attack");
		if (InterruptAttack() == false)
			return false;
		else
		{
			// If an in progress attack was interrupted, return without performing the new attack
			return true;
		}
	}

	bool Success = false;

	switch (AttackType)
	{
		case EAttackType::Primary:
		{
			if (PrimaryAttack != nullptr)
			{
				CurrentAttack = PrimaryAttack;
				Success = PrimaryAttack->StartAttack();
			}

			break;
		}
		case EAttackType::Secondary:
		{
			if (SecondaryAttack != nullptr)
			{
				CurrentAttack = SecondaryAttack;
				Success = SecondaryAttack->StartAttack();
			}

			break;
		}
		case EAttackType::AbilityOne:
		{
			if (AbilityOne != nullptr)
			{
				CurrentAttack = AbilityOne;
				Success = AbilityOne->StartAttack();
			}

			break;
		}
		case EAttackType::AbilityTwo:
		{
			if (AbilityTwo != nullptr)
			{
				CurrentAttack = AbilityTwo;
				Success = AbilityTwo->StartAttack();
			}

			break;
		}
		case EAttackType::Ultimate:
		{
			if (Ultimate != nullptr)
			{
				CurrentAttack = Ultimate;
				Success = Ultimate->StartAttack();
			}

			break;
		}
		case EAttackType::None:
		default:
		{
			break;
		}
	}

	bIsAttacking = Success;
	CurrentAttack = Success ? CurrentAttack : nullptr;

	return Success;
}

bool UAttackingComponent::IsAttackInCooldown(const EAttackType InAttackType) const
{
	switch (InAttackType)
	{
		case EAttackType::Primary:
			return PrimaryAttack != nullptr && PrimaryAttack->IsInCooldown();

		case EAttackType::Secondary:
			return SecondaryAttack != nullptr && SecondaryAttack->IsInCooldown();

		case EAttackType::AbilityOne:
			return AbilityOne != nullptr && AbilityOne->IsInCooldown();

		case EAttackType::AbilityTwo:
			return AbilityTwo != nullptr && AbilityTwo->IsInCooldown();

		case EAttackType::Ultimate:
			return Ultimate != nullptr && Ultimate->IsInCooldown();
	}

	return false;
 }

bool UAttackingComponent::InterruptAttack(bool bForceInterrupt)
{
	// No need to interrupt, if the attack is finished or is in cooldown
	if (CurrentAttack == nullptr)
		return true;
	
	// If interrupt successful, then return true
	if (CurrentAttack->InterruptAttack(bForceInterrupt))
	{
		bIsAttacking = false;
		CurrentAttack = nullptr;
		return true;
	}
	
	return false;
}

bool UAttackingComponent::CanStartAttack(const EAttackType NewAttackType, bool bAllowedInterrupt) const
{
	// TODO: Change this to incorporate new attack method (targeting, cooldown, etc.)

	// Check to see if the new attack is not in cooldown, if it is then return false
	bool bInCooldown = IsAttackInCooldown(NewAttackType);

	// No point in checking any further, if the new attack is in cooldown
	if (bInCooldown == true)
	{
		return false;
	}

	if (CurrentAttack == nullptr)
		return true;

	// Only primary and secondary attacks can be chained to perform combos
	bool bBasicAttack = (NewAttackType == EAttackType::Primary || NewAttackType == EAttackType::Secondary);

	// Can Start new attack if:
		// New Attack type is same as the current and is basic and current attack type supports attack combos or,
		// New Attack type is not basic and current attack is in targeting mode
	if ((CurrentAttack->Type == NewAttackType && bBasicAttack && CurrentAttack->CanPerformCombos()) || (CurrentAttack->IsTargeting() && CurrentAttack->Type == NewAttackType))
	{
		return true;
	}

	// If attack is in progress
	if (CurrentAttack->IsInProgress() && bAllowedInterrupt)
	{
		return CurrentAttack->CanInterrupt();
	}

	// An attack is in progress and it cannot be interrupted and nor does it allow actions
	return false;
}

bool UAttackingComponent::CanTarget(const EAttackType NewAttackType) const
{
	// Check to see if the new attack is not in cooldown, if it is then return false
	bool bInCooldown = IsAttackInCooldown(NewAttackType);

	// No point in checking any further, if the new attack is in cooldown
	if (bInCooldown == true)
		return false;

	if (CurrentAttack == nullptr)
		return true;

	return false;
}

bool UAttackingComponent::AllowsOtherActions(bool bInterruptOngoingAttack)
{
	if (CurrentAttack == nullptr)
		return true;
	if (CurrentAttack->IsTargeting())		// Cannot do anything when an attack is in targeting mode
		return InterruptAttack();
	else if (bInterruptOngoingAttack == true)
		return InterruptAttack();

	return false;
}

void UAttackingComponent::OnAttackFinished()
{
	bIsAttacking = false;
	bIsTargeting = false;

	if (CurrentAttack == nullptr)
		return;

	EAttackType Type = CurrentAttack->Type;
	CurrentAttack = nullptr;

	OwnerCharacter->PostAttackEnd(Type);
}

const FAttackStats& UAttackingComponent::GetAttackStatistics(EAttackType InType) const
{
	switch (InType)
	{
		case EAttackType::Primary:
		{
			if (PrimaryAttack != nullptr)
			{
				return PrimaryAttack->GetAttackStatistics();
			}

			break;
		}
		case EAttackType::Secondary:
		{
			if (SecondaryAttack != nullptr)
			{
				return SecondaryAttack->GetAttackStatistics();
			}

			break;
		}
		case EAttackType::AbilityOne:
		{
			if (AbilityOne != nullptr)
			{
				return AbilityOne->GetAttackStatistics();
			}

			break;
		}
		case EAttackType::AbilityTwo:
		{
			if (AbilityTwo != nullptr)
			{
				return AbilityTwo->GetAttackStatistics();
			}

			break;
		}
		case EAttackType::Ultimate:
		{
			if (Ultimate != nullptr)
			{
				return Ultimate->GetAttackStatistics();
			}

			break;
		}
	}

	return UAttack::Statistics_None;
}

void UAttackingComponent::AttackExecutionFinished()
{
	if (CurrentAttack == nullptr)
		return;

	CurrentAttack->NotifyBeginRecovery();
}

void UAttackingComponent::ExecuteCurrentAttack()
{
	// Should only be called while an attack is in progress
	CurrentAttack->ExecuteAttack();
}

void UAttackingComponent::NextAttackInCurrentAttackCombo()
{
	// Should only be called while an attack is in progress
	CurrentAttack->NotifyNextCombo();
}

void UAttackingComponent::ResetCurrentAttackCombo()
{
	// Should only be called while an attack is in progress
	CurrentAttack->NotifyResetCombo();
}

EAttackType UAttackingComponent::GetCurrentAttackType() const
{
	if (CurrentAttack != nullptr)
	{
		return CurrentAttack->Type;
	}

	return EAttackType::None;
}

EAttackState UAttackingComponent::GetCurrentAttackState() const
{
	if (CurrentAttack != nullptr)
	{
		return CurrentAttack->CurrentState;
	}

	return EAttackState::Idle;
}

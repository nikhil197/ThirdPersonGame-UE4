// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/BasicAttack.h"

#include "TimerManager.h"

#include "Attacks/AttackingComponent.h"
#include "Characters/CharacterBase.h"

UBasicAttack::UBasicAttack()
	: UAttack()
{
	NextAnimationIndex = 0;

	Statistics.DamagePerHit = 25.0f;
	Statistics.StaminaCost = 20.0f;
	Statistics.FPCost = 0.0f;
	Statistics.CooldownTime = 0;
}

bool UBasicAttack::StartAttack()
{
	bool Success = true;
	if (CurrentState == EAttackState::Progress)
	{
		// If attack is in progress, then chain the combo, move to next animation and exit
		bChainAttacks = true;
	}
	else
	{
		// If attack is not in progress, proceed normally
		// Setup the attack
		bChainAttacks = false;
		Success = Success && PreAttack();
	}

	return Success;
}

bool UBasicAttack::PreAttack()
{
	// Always true
	Super::PreAttack();
	
	return PerformAttack();
}

bool UBasicAttack::PerformAttack()
{
	// Always true
	Super::PerformAttack();

	// Actual Attack logic
	// Play the animation here
	ACharacterBase* OwnerCharacter = OwnerComponent->GetOwnerCharacter();

	float Duration = OwnerCharacter->PlayAnimMontage(Animations[NextAnimationIndex]);
	if (Duration == 0.0f)
	{
		PostAttack();
		return false;
	}

	CurrentState = EAttackState::Progress;
	bCanInterrupt = true;
	
	World->GetTimerManager().SetTimer(TimerHandle_AttackFinished, this, &UBasicAttack::PostAttack, Duration, false);

	if (bExecuteImmediately)
	{
		ExecuteAttack();
	}

	return true;
}

void UBasicAttack::PostAttack()
{
	Super::PostAttack();

	bCanInterrupt = true;
	bChainAttacks = false;
}

bool UBasicAttack::InterruptAttack(bool bForceInterrupt)
{
	if (CurrentState == EAttackState::Progress)
	{
		if (bForceInterrupt || bCanInterrupt)
		{
			World->GetTimerManager().ClearTimer(TimerHandle_AttackFinished);
		
			ACharacterBase* OwnerCharacter = OwnerComponent->GetOwnerCharacter();
			OwnerCharacter->StopAnimMontage();	// Stops the current anim montage
			OwnerCharacter->OnAttackInterrupted(Type);

			PostAttack();
		}
		else
		{
			return false;
		}
	}
	
	return true;
}

void UBasicAttack::NotifyNextCombo()
{
	if(bChainAttacks)
	{
		NextAnimationIndex = (NextAnimationIndex + 1) % Animations.Num();
		bChainAttacks = false;
		CurrentState = EAttackState::Idle;

		// Start the next combo animation
		StartAttack();
	}
}

void UBasicAttack::NotifyResetCombo()
{
	NextAnimationIndex = 0;
	bChainAttacks = false;
}

void UBasicAttack::ExecuteAttack()
{
	if (CurrentState == EAttackState::Progress)
	{
		bCanInterrupt = false;

		ACharacterBase* OwnerCharacter = OwnerComponent->GetOwnerCharacter();
		OwnerCharacter->OnAttackExecuted(Type, Statistics);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/AbilityAttack.h"

#include "TimerManager.h"

#include "Characters/CharacterBase.h"
#include "Attacks/AttackingComponent.h"

UAbilityAttack::UAbilityAttack()
	: UAttack()
{
	CurrentState = EAttackState::Idle;
	MaxKeyHeldTime = 0.0f;
	CurrentKeyHoldTime = 0.0f;

	ActiveTime = 1.0f;

	bExecuteImmediately = true;
}

void UAbilityAttack::Tick(float DeltaTime)
{
	if (CurrentState == EAttackState::Targeting)
	{
		CurrentKeyHoldTime += DeltaTime;

		// If the hold time has crossed the threshold
		if (MaxKeyHeldTime > 0.0f && CurrentKeyHoldTime >= MaxKeyHeldTime)
		{
			StartAttack();
			return;
		}
	}
}

bool UAbilityAttack::OnTarget()
{
	// To start targeting, attack state must be idle
	if (CurrentState != EAttackState::Idle)
		return false;

	// Call the blueprint version
	On_Target();

	CurrentState = EAttackState::Targeting;
	bCanInterrupt = true;

	return true;
}

bool UAbilityAttack::StopTarget()
{
	// Call the blueprint version
	Stop_Target();

	CurrentState = EAttackState::Idle;
	CurrentKeyHoldTime = 0.0f;
	return true;
}

bool UAbilityAttack::StartAttack()
{
	// To Start the attack, attack state must be targeting
	if (CurrentState != EAttackState::Targeting)
		return false;

	return PreAttack();
}

bool UAbilityAttack::PreAttack()
{
	Super::PreAttack();

	bool Success = StopTarget();
	if (Success == false)
	{
		return false;
	}
	
	Success = Success && PerformAttack();

	// Ability attacks can only be interrupted during the targeting phase
	bCanInterrupt = !Success;

	return Success;
}

bool UAbilityAttack::PerformAttack()
{
	Super::PerformAttack();

	ACharacterBase* OwnerCharacter = OwnerComponent->GetOwnerCharacter();

	float Duration = OwnerCharacter->PlayAnimMontage(AttackAnimation);
	if (Duration == 0.0f)
	{
		PostAttack();
		return false;
	}

	CurrentState = EAttackState::Progress;
	World->GetTimerManager().SetTimer(TimerHandle_AttackFinished, this, &UAbilityAttack::PostAttack, Duration, false);
	ExecuteAttack();

	return true;
}

void UAbilityAttack::PostAttack()
{
	Super::PostAttack();

	bCanInterrupt = true;
}

bool UAbilityAttack::InterruptAttack(bool bForceInterrupt)
{
	if (CurrentState == EAttackState::Progress)
	{
		// Ability attacks can only be force interrupted once in progress
		if (bForceInterrupt)
		{
			World->GetTimerManager().ClearTimer(TimerHandle_AttackFinished);
			
			ACharacterBase* OwnerCharacter = OwnerComponent->GetOwnerCharacter();
			OwnerCharacter->StopAnimMontage(AttackAnimation);
			OwnerCharacter->OnAttackInterrupted(Type);

			PostAttack();
		}
		else
		{
			return false;
		}	
	}
	else if (CurrentState == EAttackState::Targeting)
	{
		StopTarget();

		PostAttack();
	}

	bCanInterrupt = true;
	return true;
}

void UAbilityAttack::ExecuteAttack()
{
	if (CurrentState == EAttackState::Progress)
	{
		ACharacterBase* OwnerCharacter = OwnerComponent->GetOwnerCharacter();
		OwnerCharacter->OnAttackExecuted(Type, Statistics);

		if (ActiveTime > 0.0f)
		{
			World->GetTimerManager().SetTimer(TimerHandle_AbilityTimerEnd, this, &UAbilityAttack::OnAbilityTimerEnd, ActiveTime, false);
		}
	}
}

void UAbilityAttack::OnAbilityTimerEnd()
{
	// Call the blueprint version
	Ability_Timer_End();

	OwnerComponent->GetOwnerCharacter()->OnAbilityTimerEnd(Type);
}
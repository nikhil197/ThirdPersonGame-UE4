// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/Attack.h"
#include "Attacks/AttackingComponent.h"

FAttackStats UAttack::Statistics_None = FAttackStats(0.0f, FLT_MAX, FLT_MAX, FLT_MAX);

UAttack::UAttack()
{
	// These will be set in the child classes / blueprints
	Name = FName("Attack");
	Type = EAttackType::None;

	Statistics.DamagePerHit = 0.0f;
	Statistics.StaminaCost = 0.0f;
	Statistics.FPCost = 0.0f;
	Statistics.CooldownTime = 0.0f;

	CurrentState = EAttackState::Idle;

	bCanInterrupt = false;
	bExecuteImmediately = false;
}

void UAttack::Init(UWorld* InWorld, UAttackingComponent* InOwnerComponent)
{
	World = InWorld;
	OwnerComponent = InOwnerComponent;
}

bool UAttack::PreAttack()
{
	// Call the blueprint version
	Pre_Attack();

	return true;
}

bool UAttack::PerformAttack()
{
	// Call the blueprint version
	Perform_Attack();

	return true;
}

void UAttack::PostAttack()
{
	// Call the blueprint version
	Post_Attack();

	OwnerComponent->OnAttackFinished();

	// Set the timer only in case there is a cooldown time
	if (Statistics.CooldownTime > 0.0f)
	{
		World->GetTimerManager().SetTimer(TimerHandle_CooldownTimerEnd, this, &UAttack::OnFinishCooldown, Statistics.CooldownTime, false);
		CurrentState = EAttackState::Cooldown;
	}
	else
	{
		OnFinishCooldown();
	}
}

void UAttack::OnFinishCooldown()
{
	CurrentState = EAttackState::Idle;
}

void UAttack::NotifyBeginRecovery()
{
	// Can only interrupt if there is no cooldown time
	if (Statistics.CooldownTime == 0.0f)
		bCanInterrupt = true;
}
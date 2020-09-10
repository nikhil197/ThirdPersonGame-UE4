// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShinbiPlayerCharacter.h"
#include "Attacks/Attack.h"

AShinbiPlayerCharacter::AShinbiPlayerCharacter()
{
	MaxStats.HP = 200.0f;
	MaxStats.FP = 100.0f;
}

bool AShinbiPlayerCharacter::HasRequiredStats(const FAttackStats& AttackStats) const
{
	return Super::HasRequiredStats(AttackStats);
}

void AShinbiPlayerCharacter::OnAttackExecuted(const EAttackType Attack, const FAttackStats& AttackStats)
{
	Super::OnAttackExecuted(Attack, AttackStats);

	// Play sound effects, particle effects etc.
}

void AShinbiPlayerCharacter::OnAttackInterrupted(const EAttackType Attack)
{
	Super::OnAttackInterrupted(Attack);
}

void AShinbiPlayerCharacter::PostAttackEnd(const EAttackType Attack)
{
	Super::PostAttackEnd(Attack);
}
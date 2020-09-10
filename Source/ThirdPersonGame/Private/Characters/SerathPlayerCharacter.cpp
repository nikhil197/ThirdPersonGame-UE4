// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SerathPlayerCharacter.h"
#include "Attacks/Attack.h"

ASerathPlayerCharacter::ASerathPlayerCharacter()
{
	MaxStats.HP = 200.0f;
	MaxStats.Stamina = 100.0f;

	bEvil = false;
}

void ASerathPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ArmsMat_Instance = GetMesh()->CreateAndSetMaterialInstanceDynamic(ArmsMatIndex);
	ChestMat_Instance = GetMesh()->CreateAndSetMaterialInstanceDynamic(ChestMatIndex);
	TorsoMat_Instance = GetMesh()->CreateAndSetMaterialInstanceDynamic(TorsoMatIndex);
	WingWebbingMat_Instance = GetMesh()->CreateAndSetMaterialInstanceDynamic(WingWebbingMatIndex);
	WingSplineMat_Instance = GetMesh()->CreateAndSetMaterialInstanceDynamic(WingSplineMatIndex);
}

void ASerathPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASerathPlayerCharacter::HasRequiredStats(const FAttackStats& AttackStats) const
{
	return Super::HasRequiredStats(AttackStats);
}

void ASerathPlayerCharacter::OnAttackExecuted(const EAttackType AttackType, const FAttackStats& AttackStats)
{
	Super::OnAttackExecuted(AttackType, AttackStats);

	if (AttackType == EAttackType::Ultimate)
	{
		bEvil = true;
	}
}

void ASerathPlayerCharacter::OnAttackInterrupted(const EAttackType Attack)
{
	Super::OnAttackInterrupted(Attack);
}

void ASerathPlayerCharacter::PostAttackEnd(const EAttackType Attack)
{
	Super::PostAttackEnd(Attack);
}

void ASerathPlayerCharacter::OnAbilityTimerEnd(const EAttackType Attack)
{
	Super::OnAbilityTimerEnd(Attack);

	if (Attack == EAttackType::Ultimate)
	{
		bEvil = false;
	}
}
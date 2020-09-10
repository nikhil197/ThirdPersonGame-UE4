// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/Character/Serath/SerathUltimateAbility.h"

#include "TimerManager.h"

#include "Attacks/AttackingComponent.h"
#include "Characters/SerathPlayerCharacter.h"

USerathUltimateAbility::USerathUltimateAbility()
{
	Name = FName("Heresy");
}

void USerathUltimateAbility::ExecuteAttack()
{
	Super::ExecuteAttack();
}
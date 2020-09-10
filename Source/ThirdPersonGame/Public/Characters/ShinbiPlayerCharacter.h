// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "ShinbiPlayerCharacter.generated.h"

struct FAttackStats;

/**
 * 
 */
UCLASS(Blueprintable)
class THIRDPERSONGAME_API AShinbiPlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	AShinbiPlayerCharacter();

	// ACharacterBase Interface ---------------
protected:
	virtual bool HasRequiredStats(const FAttackStats& AttackStats) const override;

public:
	virtual void OnAttackExecuted(const EAttackType Attack, const FAttackStats& AttackStats) override;
	virtual void OnAttackInterrupted(const EAttackType Attack) override;
	virtual void PostAttackEnd(const EAttackType Attack) override;
	// ACharacterBase Interface --------------- End
};

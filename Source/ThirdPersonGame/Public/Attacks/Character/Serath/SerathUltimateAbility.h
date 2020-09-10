// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attacks/AbilityAttack.h"
#include "SerathUltimateAbility.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONGAME_API USerathUltimateAbility : public UAbilityAttack
{
	GENERATED_BODY()
	
public:
	USerathUltimateAbility();

	// UAttack Interface ----------------
	virtual void ExecuteAttack() override;
	// UAttack Interface ---------------- END
};

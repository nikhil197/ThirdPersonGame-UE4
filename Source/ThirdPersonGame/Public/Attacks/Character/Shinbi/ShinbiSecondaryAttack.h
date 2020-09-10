// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attacks/BasicAttack.h"
#include "ShinbiSecondaryAttack.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONGAME_API UShinbiSecondaryAttack : public UBasicAttack
{
	GENERATED_BODY()
	
public:
	UShinbiSecondaryAttack();

	// UAttack Interface
public:
	virtual void ExecuteAttack() override;

	// UAttack Interface -- End



};

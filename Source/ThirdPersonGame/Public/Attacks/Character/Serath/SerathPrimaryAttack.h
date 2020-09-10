// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attacks/BasicAttack.h"
#include "SerathPrimaryAttack.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONGAME_API USerathPrimaryAttack : public UBasicAttack
{
	GENERATED_BODY()
	
public:
	USerathPrimaryAttack();

	// UAttack Interface --------
protected:
	virtual bool PerformAttack() override;
	//virtual void PostAttack() override;
	// UAttack Interface -------- End

protected:
	/* Animation for in air attack */
	UPROPERTY(EditAnywhere, Category = "Attack|Gameplay")
	class UAnimMontage* InAirAttackAnimation;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attacks/Attack.h"
#include "BasicAttack.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDPERSONGAME_API UBasicAttack : public UAttack
{
	GENERATED_BODY()

public:
	UBasicAttack();

	// UAttack Interface --------
	virtual bool StartAttack() override;
	virtual bool InterruptAttack(bool bForceInterrupt = false) override;
	virtual void ExecuteAttack() override;
	virtual void NotifyNextCombo() override;
	virtual void NotifyResetCombo() override;
	virtual bool CanPerformCombos() const override { return Animations.Num() > 1; }

protected:
	virtual bool PreAttack() override;
	virtual bool PerformAttack() override;
	virtual void PostAttack() override;
	// UAttack Interface -------- End

protected:
	/* All the animations of the attack to cycle through (Different animation will be played depending on the 'NextAnimationIndex')*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> Animations;

protected:
	/* Whether or not to chain attacks */
	bool bChainAttacks : 1;

	/* Index of the next attack animation to be played */
	uint32 NextAnimationIndex;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attacks/Attack.h"
#include "AbilityAttack.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDPERSONGAME_API UAbilityAttack : public UAttack
{
	GENERATED_BODY()
	
public:
	UAbilityAttack();

	// UAttack Interface --------
	virtual void Tick(float DeltaTime) override;
	virtual bool StartAttack() override;
	virtual bool InterruptAttack(bool bForceInterrupt = false) override;
	virtual bool OnTarget() override;
	virtual bool StopTarget() override;
	virtual void ExecuteAttack() override;

protected:
	virtual bool PreAttack() override;
	virtual bool PerformAttack() override;
	virtual void PostAttack() override;
	// UAttack Interface -------- End

protected:
	/* Blueprint version of OnTarget() */
	UFUNCTION(BlueprintImplementableEvent)
	void On_Target();

	/* Blueprint version of StopTarget() */
	UFUNCTION(BlueprintImplementableEvent)
	void Stop_Target();

	/* Blueprint version of OnAbilityTimerEnd() */
	UFUNCTION(BlueprintImplementableEvent)
	void Ability_Timer_End();

	/* To handle the functionality when the ability timer ends */
	void OnAbilityTimerEnd();


protected:
	/* Animation for the ability attack */
	UPROPERTY(EditAnywhere, Category = "Attack|Gameplay")
	class UAnimMontage* AttackAnimation;
	
	/* Time for which the effects of the ability will be active */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Gameplay")
	float ActiveTime;

	/* Maximum time the ability key can be held before proceeding to attack (0 if no limit on the hold time) */
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Gameplay")
	float MaxKeyHeldTime;

protected:
	/* Time for which the key has been held */
	UPROPERTY(VisibleInstanceOnly, Category = "Attack|Gameplay")
	float CurrentKeyHoldTime;

private:
	/* Timer handle for handling end of the ultimate ability timer */
	FTimerHandle TimerHandle_AbilityTimerEnd;
};

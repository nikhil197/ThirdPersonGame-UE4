// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ThirdPersonGame/GameTypes.h"

#include "Attack.generated.h"

/* Various stats of an attack */
USTRUCT(BlueprintType)
struct FAttackStats
{
	GENERATED_BODY()

	/* Amount of damage dealt by the attack*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Statistics", meta = (ClampMin = "0"))
	float DamagePerHit;

	/* Amount of stamina consumed when using this attack */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Statistics", meta = (ClampMin = "0"))
	float StaminaCost;

	/* Amount of FP consumed when using this attack */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Statistics", meta = (ClampMin = "0"))
	float FPCost;
	
	/* Cool down timer for the attack (Zero for normal attacks) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Statistics", meta = (ClampMin = "0"))
	float CooldownTime;

	// TODO: Add poise damage as well

	FAttackStats()
		: DamagePerHit(0.0f), StaminaCost(0.0f), FPCost(0.0f), CooldownTime(0.0f)
	{}

	FAttackStats(float InDamagePerHit, float InStaminaCost, float InFPCost, float InCooldownTime)
		: DamagePerHit(InDamagePerHit), StaminaCost(InStaminaCost), FPCost(InFPCost), CooldownTime(InCooldownTime)
	{}


};

/**
 * Interface for the different type of attacks
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class THIRDPERSONGAME_API UAttack : public UObject
{
	GENERATED_BODY()
	
	friend class UAttackingComponent;

public:
	UAttack();

	/* Initializes the attack, creates the required object, etc. */
	virtual void Init(class UWorld* InWorld, class UAttackingComponent* InOwnerComponent);

	/* Update the target if the ability is in targeting mode */
	virtual void Tick(float DeltaTime) {};

	/* Starts this attack */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual bool StartAttack() { return false; }

	/**
	 * Interrupts this attack if the attack is still in progress. This is only possible within a specific interrupt window
	 *
	 * @param bForceInterrupt Whether to force the interrupt or not. Should be used only when the attack is interrupted by taking damage
	 */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual bool InterruptAttack(bool bForceInterrupt = false) { return false; }

	/* Start targeting the ability */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual bool OnTarget() { return false; }

	/* Stop targeting the ability */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual bool StopTarget() { return false; }

	/* Called when the attack anticipation finishes i.e. just when the attack is about to be executed and the interrupt window ends (Attack startup frames finish) */
	UFUNCTION(BlueprintCallable, Category = "Attack|Gameplay")
	virtual void ExecuteAttack() {}

	/* Changes the attack animation to the next animation in the combo, in case there are more than one animations for the attack */
	// Note: Should be called before ResetCombo() is called
	UFUNCTION(BlueprintCallable, Category = "Attack|Gameplay")
	virtual void NotifyNextCombo() {}

	/* Resets the animation to the first animation in the combo, case there are more than one animations for the attack */
	UFUNCTION(BlueprintCallable, Category = "Attack|Gameplay")
	virtual void NotifyResetCombo() {}

	/* Called when the attack execution phase finishes and the recovery begins */
	UFUNCTION(BlueprintCallable, Category = "Attack|Gameplay")
	virtual void NotifyBeginRecovery();

	/* Returns the statistics of the attack */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	const FAttackStats& GetAttackStatistics() const { return Statistics; }

	/* Returns if the attack is in progress */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool IsInProgress() const { return CurrentState == EAttackState::Progress; }

	/* Returns the attack is in cooldown */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool IsInCooldown() const { return CurrentState == EAttackState::Cooldown; }

	/* Returns if the attack is in targeting mode */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool IsTargeting() const { return CurrentState == EAttackState::Targeting; }

	/* Returns if the attack can be interrupted */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool CanInterrupt() const { return bCanInterrupt || CurrentState == EAttackState::Targeting; }

	/* Returns if this attack has combos or not */
	virtual bool CanPerformCombos() const { return false; }

protected:
	/* For all the things to be done before actually doing an attack */
	virtual bool PreAttack();

	/* Performing the attack */
	virtual bool PerformAttack();

	/* All the things to be done after an attack is finished */
	virtual void PostAttack();

	/* Blueprint version of PreAttack() method */
	UFUNCTION(BlueprintImplementableEvent)
	void Pre_Attack();

	/* Blueprint version of PerformAttack() method */
	UFUNCTION(BlueprintImplementableEvent)
	void Perform_Attack();

	/* Blueprint version of PostAttack() method */
	UFUNCTION(BlueprintImplementableEvent)
	void Post_Attack();

	/* Returns the world of which this attack is a part of */
	virtual class UWorld* GetWorld() const { return World; }

	/* Called when the cooldown timer for the attack ends */
	virtual void OnFinishCooldown();

public:
	/* Invalid Statistics */
	static FAttackStats Statistics_None;

public:
	/* Name of the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FName Name;
	
	/* Type of the attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	EAttackType Type;

protected:
	/* Component that owns this attack */
	UPROPERTY(BlueprintReadOnly)
	class UAttackingComponent* OwnerComponent;

	/* World this attack is a part of */
	UPROPERTY(Transient)
	class UWorld* World;

	/* Statistics of this attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FAttackStats Statistics;

	/* Timer handle to invoke some functionality once the attack if finished */
	FTimerHandle TimerHandle_AttackFinished;

	/* Timer handle to invoke some functionality when the cooldown timer for the attack ends */
	FTimerHandle TimerHandle_CooldownTimerEnd;

	/* Current state of the attack */
	EAttackState CurrentState;

	/**
	 * If the attack can be interrupted (the interrupt window starts at the beginning of the attack and ends when the attack is actually performed)
	 * 
	 * @see OnAttackAnticipationFinish()
	 */
	bool bCanInterrupt : 1;

	/*
	* Execute the attack immediately after it begins
	*/
	bool bExecuteImmediately : 1;
};

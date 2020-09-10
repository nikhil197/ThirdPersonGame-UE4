// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attacks/AbilityAttack.h"
#include "SerathAbilityTwo.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONGAME_API USerathAbilityTwo : public UAbilityAttack
{
	GENERATED_BODY()
public:
	USerathAbilityTwo();

	// UAttack Interface --------
	virtual void Init(class UWorld* InWorld, class UAttackingComponent* InOwnerComponent) override;
	virtual bool OnTarget() override;
	virtual bool StopTarget() override;
	virtual void ExecuteAttack() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual bool PreAttack() override;
	virtual void PostAttack() override;
	// UAttack Interface -------- End

private:
	/* Update the target decal transform */
	void UpdateDecalTransform(float DeltaTime, bool bStartedTargeting = false);

	/* Called when the rise animation finishes */
	void OnReachedSetHeight();

protected:
	/* Maximum distance up to which the ability can target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityTwo")
	float TargetingDistance;

	/* Maximum height up to which the character rises during targeting mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityTwo")
	float TargetingHeight;

	/* Material used for the Target decal */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityTwo")
	UMaterial* TargetDecalMaterial;

	/* Animation for when the character rises in the air */
	UPROPERTY(EditAnywhere, Category = "Gameplay|AbilityTwo")
	class UAnimMontage* RiseAnimation;

	/* Speed to interpolate to the targeting height location */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityTwo")
	float FRiseInterpSpeed;

private:
	/* Decal spawned to show the targeted area */
	UPROPERTY(VisibleInstanceOnly, Category = "Gameplay|AbilityTwo")
	class ADecalActor* TargetDecal;

	/* Timer handle to handle the timer for the rise animation */
	FTimerHandle TimerHandle_OnReachedSetHeight;

	/* Position in the air where the character will rise to */
	FVector InAirLocation;

	/* Position where the character will land when the attack finishes */
	FVector AttackLocation;

	/* Speed to interpolate to the targeting height location */
	float RiseInterpSpeed;

	/* Speed to interpolate to the attack location */
	float AttackInterpSpeed;

	/* Whether the character is rising in the air */
	bool bRising : 1;

	/* Whether to continue to attack immediately after reaching targeting height */
	bool bContinueToAttack : 1;

	/* Max frame rate Sample for interpolating between positions */
	const float MaxFrameRateSample = 1.0f / 60.0f;
};

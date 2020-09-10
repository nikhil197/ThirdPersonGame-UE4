// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ThirdPersonGame/GameTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 *
 */
UCLASS()
class THIRDPERSONGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	void SetAIState(EAIState NewState);

	EAIState GetAIState() const;

	void SetDestination(const FVector& Destination);

	FVector GetDestination() const;

	void SetTarget(AActor* Target);

	const class AActor* GetTarget() const;

	void SetLastKnownLocation(const FVector& Loc);

	FVector GetLastKnownLocation() const;

	bool DoesAINeedAmmo() const;

	bool DoesAINeedHealth() const;

	void SetAIToFollow(class AActor* AI);

private:
	//// AController Interface Begin
	virtual void OnPossess(class APawn* PossessedPawn) override;

	virtual void OnUnPossess() override;

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;
	//// AController Interface End

	/* Handles the search time over event */
	void OnSearchTimeOver();

	/* Handles the investigation time over event */
	void OnInvestigationTimeOver();

	/* Handles the suspicion meter full event */
	void OnSuspicionMeterFull(class AActor* Target);

	/* Delegate to handle the target perception update event */
	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", META = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", META = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Damage* DamageConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", META = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* SightConfig;

	/* Name for the Current AI State key in the blackboard */
	FName StateKeyName;

	/* Name for the current destination key in the blackboard */
	FName DestinationKeyName;

	/* Name for the Target key in the blackboard */
	FName TargetKeyName;

	/* Name for the last known location key in the blackboard */
	FName LastKnownLocKeyName;

	/* Name for the Need Ammo key in the blackboard */
	FName NeedAmmoKeyName;

	/* Name for the Need Health key in the blackboard */
	FName NeedHealthKeyName;

	/* Name for the AI To Follow key in the blackboard */
	FName AIToFollowKeyName;

	/* Current sound source that is being investigated */
	ESoundSource::Source CurrentInvestigatedSource;

	/* Time for which the AI will search around the area to look for the enemy */
	float SearchingTime = 20.0f;

	/* Time for which the AI will investigate a stimulus */
	float InvestigationTime = 10.0f;

	/* Time for which the AI will look at the player with suspecion before attacking */
	float SuspicionMeterTime = 2.5f;

	/* Timer handle to call the delegate when search time is over */
	FTimerHandle TimerHandle_SearchTimeExpired;

	/* Timer handle to call the delegate when investigation time is over */
	FTimerHandle TimerHandle_InvestigationTimeExpired;

	/* Timer Handle to call the delegate when suspicion meter fills */
	FTimerHandle TimerHandle_SuspicionMeterFull;
};

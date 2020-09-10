// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SerathGhost.generated.h"

UCLASS(Blueprintable)
class THIRDPERSONGAME_API ASerathGhost
	: public AActor
{
	GENERATED_BODY()

public:
	ASerathGhost();

	/* Plays animation of the ghost */
	float PlayAnimation(class UAnimMontage* AnimMontage);

private:
	/* Called when the animation finishes */
	void AnimationFinished();

	/* Called when Time to live expires */
	void TimeToLiveExpired();

public:
	/* Whether to use alternate animation or not */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUseAlt;

protected:
	/* Time for which the ghost will be alive (After the animation finishes) */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|AbilityOne|Ghosts")
	float TimeToLive;

private:
	/** The main skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	/* Timer handle to hide the ghost when animation finishes */
	FTimerHandle TimerHandle_AnimFinished;

	/* Timer handle to hide the ghost when time to live has expired for the ghost */
	FTimerHandle TimerHandle_TimeToLiveExpired;
};
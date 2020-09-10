// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAssist/SerathGhost.h"

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

ASerathGhost::ASerathGhost()
{
	SetActorHiddenInGame(true);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GhostMesh"));
	SetRootComponent(Mesh);

	bUseAlt = false;
	TimeToLive = 0.2f;
}

float ASerathGhost::PlayAnimation(UAnimMontage* AnimMontage)
{
	// Un-hide the actor first
	SetActorHiddenInGame(false);

	float Duration = 0.0f;
	UAnimInstance* AnimInstance = (Mesh) ? Mesh->GetAnimInstance() : nullptr;
	if (AnimMontage && AnimInstance)
	{
		Duration = AnimInstance->Montage_Play(AnimMontage, 1.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_AnimFinished, this, &ASerathGhost::AnimationFinished, Duration, false);
	}

	return Duration;
}

void ASerathGhost::AnimationFinished()
{
	GetWorldTimerManager().SetTimer(TimerHandle_TimeToLiveExpired, this, &ASerathGhost::TimeToLiveExpired, TimeToLive, false);
}

void ASerathGhost::TimeToLiveExpired()
{
	SetActorHiddenInGame(true);
}
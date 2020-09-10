// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState_TimedParticleEffect.h"
#include "Serath_TimedParticleEffect.generated.h"

class UAnimSequenceBase;
class UParticleSystem;
class USkeletalMeshComponent;
class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class THIRDPERSONGAME_API USerath_TimedParticleEffect : public UAnimNotifyState_TimedParticleEffect
{
	GENERATED_BODY()
	
	// The particle system template to use when spawning the particle component in normal / holy mode
	UPROPERTY(EditAnywhere, Category = ParticleSystem, meta = (ToolTip = "The particle system to spawn for the notify state in Holy mode", DisplayName = "Holy Particle System"))
	UParticleSystem* PS_Holy;

	// The particle system template to use when spawning the particle component in evil mode
	UPROPERTY(EditAnywhere, Category = ParticleSystem, meta = (ToolTip = "The particle system to spawn for the notify state in Evil Mode", DisplayName = "Evil Particle System"))
	UParticleSystem* PS_Evil;

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;
};

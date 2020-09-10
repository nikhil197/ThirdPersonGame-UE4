// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlayParticleEffect.h"
#include "Serath_PlayParticleEffect.generated.h"

class UAnimSequenceBase;
class UParticleSystem;
class USkeletalMeshComponent;
class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class THIRDPERSONGAME_API USerath_PlayParticleEffect : public UAnimNotify_PlayParticleEffect
{
	GENERATED_BODY()
	
public:
	// Begin UAnimNotify interface
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	// End UAnimNotify interface

	// Particle System to Spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(DisplayName="Holy Particle System"))
	UParticleSystem* PS_Holy;

	// Particle System to Spawn in Evil Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(DisplayName="Fallen Particle System"))
	UParticleSystem* PS_Evil;
};

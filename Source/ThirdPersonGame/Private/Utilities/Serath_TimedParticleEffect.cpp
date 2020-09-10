// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/Serath_TimedParticleEffect.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/SerathPlayerCharacter.h"

void USerath_TimedParticleEffect::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	ASerathPlayerCharacter* SerathCharacter = Cast<ASerathPlayerCharacter>(MeshComp->GetOwner());
	if (SerathCharacter != nullptr)
	{
		if (SerathCharacter->InEvilMode())
		{
			PSTemplate = PS_Evil;
		}
		else
		{
			PSTemplate = PS_Holy;
		}
	}
	else
	{
		PSTemplate = PS_Holy;
	}

	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}
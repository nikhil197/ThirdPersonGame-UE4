// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/Serath_PlayParticleEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"

#include "Characters/SerathPlayerCharacter.h"

void USerath_PlayParticleEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ASerathPlayerCharacter* SerathCharacter = Cast<ASerathPlayerCharacter>(MeshComp->GetOwner());
	if(SerathCharacter != nullptr)
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
	
	SpawnParticleSystem(MeshComp, Animation);
}
#include "Utilities/Serath_Trail.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/SerathPlayerCharacter.h"

void USerath_Trail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
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
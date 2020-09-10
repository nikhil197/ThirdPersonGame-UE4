// Copyright Epic Games, Inc. All Rights Reserved.


#include "ThirdPersonGameGameModeBase.h"
#include "UObject/ConstructorHelpers.h"

#include "CharacterController.h"

AThirdPersonGameGameModeBase::AThirdPersonGameGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(TEXT("/Game/Blueprints/PlayerCharacters/BP_SerathPlayerCharacter"));
	if (PlayerPawnClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnClass.Class;
	}

	PlayerControllerClass = ACharacterController::StaticClass();
}
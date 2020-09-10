// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONGAME_API ACharacterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACharacterController();

	// APlayerController Interface ------------
	virtual void SetupInputComponent() override;
	virtual void UpdateRotation(float DeltaTime) override;
	// APlayerController Interface ------------ END

	/* Return if the game is paused or not */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsGamePaused() const { return bIsGamePaused; }

private:
	/* When the game is paused */
	void OnPause();

public:
	/* If the controller rotation changed in the current frame */
	bool bChangedRotation;

private:
	/* Whether the game is paused */
	bool bIsGamePaused : 1;

};

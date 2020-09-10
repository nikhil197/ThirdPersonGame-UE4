// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"

#include "Engine.h"

ACharacterController::ACharacterController()
{

}

void ACharacterController::UpdateRotation(float DeltaTime)
{
	Super::UpdateRotation(DeltaTime);

	if (RotationInput != FRotator::ZeroRotator)
	{
		bChangedRotation = true;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, RotationInput.ToString());
}

void ACharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Setup controls common for all characters like pause menu, save, etc.
	InputComponent->BindAction("Pause", IE_Pressed, this, &ACharacterController::OnPause);
}

void ACharacterController::OnPause()
{

}
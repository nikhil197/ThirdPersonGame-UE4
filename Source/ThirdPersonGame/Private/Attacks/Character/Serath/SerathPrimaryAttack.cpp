// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/Character/Serath/SerathPrimaryAttack.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Attacks/AttackingComponent.h"
#include "Characters/CharacterBase.h"

USerathPrimaryAttack::USerathPrimaryAttack()
{
	Name = FName("Angelic Smite");
}

bool USerathPrimaryAttack::PerformAttack()
{
	UAttack::PerformAttack();

	// Actual Attack logic
	// Play the animation here
	ACharacterBase* OwnerCharacter = OwnerComponent->GetOwnerCharacter();

	float Duration = 0.0f;
	if (OwnerCharacter->GetCharacterMovement()->IsFalling())
		Duration = OwnerCharacter->PlayAnimMontage(InAirAttackAnimation);
	else
		Duration = OwnerCharacter->PlayAnimMontage(Animations[NextAnimationIndex]);

	if (Duration == 0.0f)
	{
		PostAttack();
		return false;
	}
	
	CurrentState = EAttackState::Progress;
	bCanInterrupt = true;

	World->GetTimerManager().SetTimer(TimerHandle_AttackFinished, this, &USerathPrimaryAttack::PostAttack, Duration, false);

	return true;
}
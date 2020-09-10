// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/Character/Serath/SerathAbilityOne.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/DecalActor.h"
#include "TimerManager.h"

#include "Attacks/AttackingComponent.h"
#include "Characters/SerathPlayerCharacter.h"
#include "CharacterController.h"
#include "CharacterAssist/SerathGhost.h"

USerathAbilityOne::USerathAbilityOne()
{
	Name = FName("Heaven's Fury");

	NumGhostsToSpawn = 3;
	TargetingDistance = 1000.0f;
	TargetRadius = 200.0f;
	GhostSpawnDelay = 0.1f;
}

void USerathAbilityOne::Init(UWorld* InWorld, UAttackingComponent* InOwnerComponent)
{
	Super::Init(InWorld, InOwnerComponent);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = InOwnerComponent->GetOwnerCharacter();

	// Spawn the ghosts
	for (uint32 i = 0; i < NumGhostsToSpawn; i++)
	{
		ASerathGhost* Ghost = World->SpawnActor<ASerathGhost>(GhostClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		Ghosts.Add(Ghost);
	}

	TargetDecal = World->SpawnActor<ADecalActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	TargetDecal->SetDecalMaterial(TargetDecalMaterial);		// Okay to set here because this will be called from the attacking component BeginPlay()
	TargetDecal->SetActorHiddenInGame(true);
	TargetDecal->SetActorScale3D(2 * FVector(1.0f));
}

bool USerathAbilityOne::OnTarget()
{
	TargetDecal->SetActorHiddenInGame(false);
	UpdateDecalTransform(0.0f, true);

	return Super::OnTarget();
}

bool USerathAbilityOne::StopTarget()
{
	// Hide the target decal
	TargetDecal->SetActorHiddenInGame(true);

	return Super::StopTarget();
}

void USerathAbilityOne::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EAttackState::Targeting)
	{
		ACharacterBase* Character = OwnerComponent->GetOwnerCharacter();
		ACharacterController* CharacterController = Cast<ACharacterController>(Character->GetController());
		if (CharacterController != nullptr)
		{
			// check if we need to move the decal if the controller rotation changed
			if (CharacterController->bChangedRotation == true || Character->bChangedPosition == true)
			{
				// Update the target transform
				UpdateDecalTransform(DeltaTime);

				Character->bChangedPosition = false;
				CharacterController->bChangedRotation = false;
			}
		}
	}
}

void USerathAbilityOne::ExecuteAttack()
{
	Super::ExecuteAttack();

	ACharacterBase* Character = OwnerComponent->GetOwnerCharacter();
	Character->SetCanMove(false);
	Character->SetFinishedJump(false);

	// Spawn Ghosts / Make the ghosts visible again (for optimization, spawn only once and hide when time to destroy)
	if (GhostSpawnDelay > 0.0f)
	{
		World->GetTimerManager().SetTimer(TimerHandle_SpawnGhostDelegate, this, &USerathAbilityOne::SpawnGhosts, GhostSpawnDelay, false);
	}
	else
	{
		SpawnGhosts();
	}
}

void USerathAbilityOne::PostAttack()
{
	Super::PostAttack();

	ACharacterBase* Character = OwnerComponent->GetOwnerCharacter();
	Character->SetCanMove(true);
	Character->SetFinishedJump(true);
}

void USerathAbilityOne::SpawnGhosts()
{
	FVector SpawnLocation = TargetDecal->GetActorLocation();

	int32 index = 0;
	int32 NumGhosts = 1;
	Delegate.BindUFunction(this, FName("PlayGhostAnimation"), index, NumGhosts, SpawnLocation);

	World->GetTimerManager().SetTimer(TimerHandle_SpawnGhostDelegate, Delegate, GhostSpawnDelay, false);
}

void USerathAbilityOne::PlayGhostAnimation(int32 index, const int32 NumGhosts, const FVector& Location)
{
	if (index == NumGhosts)
		return;

	ASerathGhost* Ghost = Ghosts[index];
	Ghost->SetActorLocation(Location);
	float Duration = 0.0f;
	if (index & 1)
	{
		Ghost->bUseAlt = true;
		Duration = Ghost->PlayAnimation(GhostAltAnimations[index]);
	}
	else
	{
		Ghost->bUseAlt = false;
		Duration = Ghost->PlayAnimation(GhostAnimations[index]);
	}

	index = index + 1;

	Delegate.BindUFunction(this, FName("PlayGhostAnimation"), index, NumGhosts, Location);
	if (Duration > 0.0f)
	{
		World->GetTimerManager().SetTimer(TimerHandle_SpawnGhostDelegate, Delegate, Duration, false);
	}
}

void USerathAbilityOne::UpdateDecalTransform(float DeltaTime, bool bStartedTargeting)
{
	ASerathPlayerCharacter* Character = Cast<ASerathPlayerCharacter>(OwnerComponent->GetOwnerCharacter());
	if (Character != nullptr)
	{
		USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
		FQuat SocketRotation = CharacterMesh->GetSocketQuaternion(Character->EyesSocketName);
		FVector Direction = FMath::Lerp(SocketRotation.GetRightVector(), CharacterMesh->GetRightVector(), 0.5f);
		Direction.Normalize();
		FVector Location = Character->GetActorLocation() + Direction * 1000;

		TargetDecal->SetActorLocationAndRotation(Location, Character->GetActorUpVector().Rotation());
	}
}
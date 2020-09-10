// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attacks/AbilityAttack.h"
#include "SerathAbilityOne.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class THIRDPERSONGAME_API USerathAbilityOne : public UAbilityAttack
{
	GENERATED_BODY()
	
public:
	USerathAbilityOne();

	// UAttack Interface --------
	virtual void Init(class UWorld* InWorld, class UAttackingComponent* InOwnerComponent) override;
	virtual bool OnTarget() override;
	virtual bool StopTarget() override;
	virtual void ExecuteAttack() override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void PostAttack() override;
	// UAttack Interface -------- End
	
private:
	// Spawn the ghosts
	void SpawnGhosts();

	// Play animation of each individual ghost spawned
	UFUNCTION()
	void PlayGhostAnimation(int32 index, const int32 NumGhosts, const FVector& Location);

	// Updates the location of the target decal
	void UpdateDecalTransform(float DeltaTime, bool bStartedTargeting = false);

protected:
	/* Number of ghosts to spawn in the attack */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|AbilityOne")
	uint32 NumGhostsToSpawn;

	/* Delay between spawning each ghost */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityOne")
	float GhostSpawnDelay;

	/* Maximum distance up to which the ability can target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityOne")
	float TargetingDistance;

	/* Maximum height above the surface to which the character will rise while targeting this ability */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityOne")
	float MaxHeight;

	/* Radius of the target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityOne")
	float TargetRadius;

	/* Class of the ghost to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityOne")
	TSubclassOf<class ASerathGhost> GhostClass;

	/* Animations used by the assisting ghosts */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityOne", meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> GhostAnimations;

	/* Alternate Animations used by the assisting ghosts */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityOne", meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> GhostAltAnimations;

	/* Material used for the Target decal */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|AbilityOne")
	UMaterial* TargetDecalMaterial;

private:
	/* Timer handle for handling the spawning of ghosts at pre-specified interval */
	FTimerHandle TimerHandle_SpawnGhostDelegate;

	/* A Timer delegate to bind params to the called function to spawn the ghosts */
	FTimerDelegate Delegate;

	/* Ghosts spawned in the world */
	UPROPERTY(VisibleInstanceOnly, Category = "Gameplay|AbilityOne")
	TArray<class ASerathGhost*> Ghosts;

	/* Decal spawned to show the targeted area */
	UPROPERTY(VisibleInstanceOnly, Category = "Gameplay|AbilityOne")
	class ADecalActor* TargetDecal;

	/* Current height of the character above the surface */
	UPROPERTY(VisibleInstanceOnly, Category = "Gameplay|AbilityOne")
	float CurrentHeight;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonGame/GameTypes.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class THIRDPERSONGAME_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Called after all components are initialised */
	virtual void PostInitializeComponents() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Returns the behavior tree used for the soldier */
	inline class UBehaviorTree* GetBehavior() const { return BehaviorTree; }

	/* Returns the current AI State */
	inline EAIState GetCurrentAIState() const { return CurrentState; }

	/* Returns the current health of the soldier */
	inline float GetCurrentHealth() const { return HitPoints; }

	/* Returns the max health of the soldier */
	inline float GetMaxHealth() const { return MaxHitPoints; }

protected:

	/* Max Hit points of the Soldier */
	UPROPERTY(EditDefaultsOnly, Category = Attributes)
	float MaxHitPoints;

private:
	/* Current Hit points of the AI Soldier */
	float HitPoints;

	/* Current state the AI is in */
	EAIState CurrentState;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	class UBehaviorTree* BehaviorTree;
};
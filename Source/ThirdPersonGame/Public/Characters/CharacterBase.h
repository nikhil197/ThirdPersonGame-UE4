// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "ThirdPersonGame/GameTypes.h"

#include "CharacterBase.generated.h"

/* All the statistics of a character like health, stamina, etc. */
USTRUCT(BlueprintType, meta = (ToolTip = "All the statistics of a character like health, stamina, etc."))
struct FCharacterStats
{
	GENERATED_BODY()

	/* Maximum health of the character */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attributes", meta = (ClampMin = "0"))
	float HP = 100.0f;

	/* Maximum stamina of the character */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attributes", meta = (ClampMin = "0"))
	float Stamina = 100.0f;
	 
	/* Maximum Focus points (Mana) of the character */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attributes", meta = (ClampMin = "0"))
	float FP = 100.0f;

	/* Amount of HP Recovered every second */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Recovery", meta = (ClampMin = "0"))
	float HPRecovery = 1.0f;

	/* Amount of stamina recovered every second */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Recovery", meta = (ClampMin = "0"))
	float StaminaRecovery = 10.0f;

	/* Amount of FP recovered every second */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Recovery", meta = (ClampMin = "0"))
	float FPRecovery = 1.0f;
};

/* Different stat Cost of each character action */
USTRUCT(BlueprintType, meta = (ToolTip = "Different stat costs of each action performed by a character like Sprinting, jumping, etc."))
struct FCharacterActionStatCost
{
	GENERATED_BODY()

	/* Amount of stamina consumed every second when sprinting */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Stamina", meta = (ClampMin = "0"))
	float SprintStaminaCost = 5.0f;

	/* Amount of Stamina consumed while jumping */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Stamina", meta = (ClampMin = "0"))
	float JumpStaminaCost = 15.0f;
};

struct FAttackStats;

UCLASS(Abstract, Blueprintable)
class THIRDPERSONGAME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* When the character finishes the jump animation and hits the ground */
	virtual void Landed(const FHitResult& Hit) override;

public:
	/**
	 * Called when the attack is actually executed
	 *
	 * @param Attack Attack that is being executed
	 * @param AttackStats Statistics of the attack executed
	 */
	virtual void OnAttackExecuted(const EAttackType Attack, const FAttackStats& AttackStats);

	/**
	 * Called when an in progress is interrupted
	 *
	 * @param Attack Attack that was interrupted
	 */
	virtual void OnAttackInterrupted(const EAttackType Attack);

	/**
	 * Called when an ability enter targeting mode
	 *
	 * @param Ability Ability which entered targeting mode
	 */
	virtual void OnStartTargeting(const EAttackType Ability);

	/* Called after any attack finishes (all the animations and stuff is done) */
	virtual void PostAttackEnd(const EAttackType Attack);

	/* Called when the effects of the specified ability are finished */
	virtual void OnAbilityTimerEnd(const EAttackType Attack);

	/* Returns the maximum values for the basic character stats */
	FORCEINLINE const FCharacterStats& GetMaximumCharacterStats() const { return MaxStats; }

	/* Returns the camera component */
	FORCEINLINE class UCameraComponent* GetCamera() const { return ThirdPersonCamera; }

	/* Returns the camera follow thread */
	FORCEINLINE class USpringArmComponent* GetCameraFollowThread() const { return CameraBoom; }

	/* Sets new value for the finished jump */
	FORCEINLINE void SetFinishedJump(bool bNewFinishedJump) { bFinishedJump = bNewFinishedJump; }

	/* Returns whether or not the character can move */
	FORCEINLINE bool CanMove() const { return bCanMove; }

	/* Sets whether the character can move or not */
	FORCEINLINE void SetCanMove(bool bNewCanMove) { bCanMove = bNewCanMove; }

	/* Returns if the character is running (sprinting) */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsRunning() const { return bIsRunning; }

	/* Called via blueprints when the jump recovery is finished */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnJumpRecoveryFinished();

	/* Returns if the character's ability one is in targeting mode */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool IsTargetingAbilityOne() const;

	/* Returns if the character's ability two is in targeting mode */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool IsTargetingAbilityTwo() const;

	/* Returns if the character's ultimate ability is in targeting mode */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool IsTargetingUltimate() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	/* Whether the character can start the new specified attack type, i.e. fulfills all the necessary conditions or not */
	virtual bool CanStartAttack(const EAttackType NewAttackType) const;

	/* Checks if the character has the required stats to perform the attack with the given stats */
	virtual bool HasRequiredStats(const FAttackStats& AttackStats) const;

protected:
	/* Setups up a certain amount of delay, before stats can recover again with every tick */
	void SetupStatsRecoveryDelay();

	// --------- Input Delegates ----------

	/* Called via input to perform a jump */
	virtual void OnJumpStart();
	
	/* Called via input to stop jumping */
	void OnJumpStop();

	/* Called via input to stop sprint */
	void OnSprintStop();

private:
	
	/* Called via input to start sprint */
	void OnSprintStart();

	/* Called via input to perform primary attack */
	void OnPrimaryAttack();

	/* Called via input to perform secondary attack */
	void OnSecondaryAttack();

	/* Called via input to start ability one targeting */
	void OnAbilityOneTarget();

	/* Called via input to ability one attack */
	void OnAbilityOneAttack();

	/* Called via input to start ability two targeting */
	void OnAbilityTwoTarget();

	/* Called via input to perform ability two attack */
	void OnAbilityTwoAttack();

	/* Called via input to start ultimate ability targeting */
	void OnUltimateAbilityTarget();

	/* Called via input to perform ultimate ability attack */
	void OnUltimateAbilityAttack();

	/* Moves the character forward / backward */
	void MoveForward(float Value);

	/* Moves the character right / left */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnRate(float Value);

	// -------- Input Delegates ---------- ~END

public:
	/* If the character position has changed since it was last processed */
	bool bChangedPosition;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/* Delay (in seconds) in consumption of stamina when character starts running, in case the stamina is full */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float SprintConsumptionDelay;

	/* Delay (in seconds) in recovery of character when character stops running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float StatsRecoveryDelay;

	/* Time (in seconds) required for recovering from a jump */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	float MaxJumpRecoveryTime;

	/* Name of the socket in the mesh representing eyes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	FName EyesSocketName;

protected:
	/* Maximum values of the base character stats */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	FCharacterStats MaxStats;

	/* Current values of the various character stats */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
	FCharacterStats CurrentStats;

	/* Different stat cost of each action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	FCharacterActionStatCost StatsCost;

	/* Attacking component which will handle all the different attacks a character can do */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UAttackingComponent* AttackingComponent;

	/* Third Person Camera to follow the character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCamera;

	/* Camera Boom to position and move the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

protected:
	/* Whether the character can move or not */
	bool bCanMove : 1;
	
private:
	/* Timer handle to handle the delegate to add a delay to stamina consumption while sprinting */
	FTimerHandle TimerHandle_SprintDelay;

	/* Timer handle to handle the delegate to add a delay to stats recovery when character stops sprinting */
	FTimerHandle TimerHandle_RecoverStatsDelay;

	/* Timer handle to handle the delegate for recovering from a jump */
	FTimerHandle TimerHandle_JumpRecoveryFinished;

	/* Whether the character is running */
	bool bIsRunning : 1;

	/* Whether the character has finished the previous jump (including land and jump animations) */
	bool bFinishedJump : 1;

	/* Whether to use up stamina when sprinting */
	bool bUseStaminaWhenSprinting : 1;

	/* Whether to recover stats or not (To add a delay in recovering the stats, just like stamina usage delay) */
	bool bRecoverStats : 1;

};

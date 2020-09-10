// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ThirdPersonGame/GameTypes.h"
#include "Attacks/Attack.h"

#include "AttackingComponent.generated.h"

/**
* This Component is responsible for handling all the attacks that a character can do
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSONGAME_API UAttackingComponent 
	: public UActorComponent
{
	GENERATED_BODY()

	friend class UAttack;
	friend class ACharacterBase;

public:	
	// Sets default values for this component's properties
	UAttackingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Starts targeting mode of an attack.
	 * Only Ability attacks can be used in a targeting mode
	 */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool OnTarget(const EAttackType NewAttackType);

	/* Starts an attack of provided type. Returns if the attack was successfully started */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool OnAttack(const EAttackType NewAttackType = EAttackType::None);

	/* Returns if the specified attack is in cooldown or not */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool IsAttackInCooldown(const EAttackType InAttackType) const;

	/**
	 * Interrupts the current attack. Returns if the attack was successfully interrupted (If there was an attack in progress)
	 *
	 * @param bForceInterrupt Whether to force the interrupt or not. Should be used only when the attack is interrupted by taking damage
	 */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool InterruptAttack(bool bForceInterrupt = false);

	/**
	 * Returns whether the component can start attack
	 *
	 * @param bAllowedInterrupt Whether interrupting an attack is allowed
	 */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool CanStartAttack(const EAttackType NewAttackType, bool bAllowedInterrupt = true) const;

	/**
	 * Return whether the owner can target using an attack of not
	 *
	 *	@param NewAttackType New attack that is to be used
	 */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool CanTarget(const EAttackType NewAttackType) const;

	/**
	 * Returns whether the attacking component can allow owner character to perform actions like jump, sprint, etc. or not (Depends on other conditions as well)
	 * 
	 * @param bInterruptOngoingAttack Whether or not to interrupt the ongoing attack (if necessary and possible), if any
	 */
	UFUNCTION(BlueprintCallable, Category = "Attacking|Gameplay")
	bool AllowsOtherActions(bool bInterruptOngoingAttack = false);

	/* Returns whether the component is attacking */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool IsAttacking() const { return bIsAttacking; }

	/* Returns whether the component is targeting or not */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	bool IsTargeting() const { return bIsTargeting; }

	/* Returns the statistics of the specified attack type */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	const struct FAttackStats& GetAttackStatistics(EAttackType InType) const;

	/* Returns the current attack that is in progress */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	class UAttack* GetCurrentAttack() const { return CurrentAttack; }

	/* Returns the current attack type in progress */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	EAttackType GetCurrentAttackType() const;

	/* Returns the current attack state */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	EAttackState GetCurrentAttackState() const;

	/* Executes the current attack. Called when the attack anticipation (interrupt window) ends. */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	void ExecuteCurrentAttack();

	/* Called When the attack execution is finished (Recovery of an attack starts. i.e. Attack can be interrupted now)*/
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	void AttackExecutionFinished();

	/* Plays the next attack animation in the combo of current attack */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	void NextAttackInCurrentAttackCombo();
	
	/* Reset the combo of the current attack */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	void ResetCurrentAttackCombo();

	/* Returns the owner character of this component */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	class ACharacterBase* GetOwnerCharacter() const { return OwnerCharacter; }
	
	/* Returns the primary attack object */
	FORCEINLINE class UAttack* GetPrimaryAttack() const { return PrimaryAttack; }

	/* Returns the secondary attack object */
	FORCEINLINE class UAttack* GetSecondaryAttack() const { return SecondaryAttack; }

	/* Returns the ability one attack object */
	FORCEINLINE class UAttack* GetAbilityOne() const { return AbilityOne; }

	/* Returns the ability two attack object */
	FORCEINLINE class UAttack* GetAbilityTwo() const { return AbilityTwo; }

	/* Returns the ultimate ability attack object */
	FORCEINLINE class UAttack* GetUltimate() const { return Ultimate; }

private:
	/* Called the attack in progress, when the attack finishes */
	void OnAttackFinished();

private:
	/* Whether the owner character is attacking or not */
	bool bIsAttacking : 1;

	/* Whether the owner character is targeting an attack or not */
	bool bIsTargeting : 1;

	/* Current attack that is in progress (NULL if no attack is in progress) */
	class UAttack* CurrentAttack;

	/* Owner Character of this component */
	class ACharacterBase* OwnerCharacter;

protected:
	/* Primary attack of the character  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attacks")
	TSubclassOf<class UBasicAttack> PrimaryAttackClass;
		
	/* Secondary attack of the character  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attacks")
	TSubclassOf<class UBasicAttack> SecondaryAttackClass;

	/* First ability of the character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attacks|Abilities")
	TSubclassOf<class UAbilityAttack> AbilityOneClass;

	/* Second ability of the character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attacks|Abilities")
	TSubclassOf<class UAbilityAttack> AbilityTwoClass;

	/* Third (Ultimate) ability class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attacks|Abilities")
	TSubclassOf<class UAbilityAttack> UltimateClass;

private:
	/* Primary attack of the character  */
	UPROPERTY(VisibleInstanceOnly, Category = "Attacks")
	class UAttack* PrimaryAttack = nullptr;
		
	/* Secondary attack of the character  */
	UPROPERTY(VisibleInstanceOnly, Category = "Attacks")
	class UAttack* SecondaryAttack = nullptr;

	/* First ability of the character */
	UPROPERTY(VisibleInstanceOnly, Category = "Attacks")
	class UAttack* AbilityOne = nullptr;

	/* Second ability of the character */
	UPROPERTY(VisibleInstanceOnly, Category = "Attacks")
	class UAttack* AbilityTwo = nullptr;

	/* Third (Ultimate) ability of the character */
	UPROPERTY(VisibleInstanceOnly, Category = "Attacks")
	class UAttack* Ultimate = nullptr;
};

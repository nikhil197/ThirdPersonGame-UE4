// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "SerathPlayerCharacter.generated.h"

struct FAttackStats;

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDPERSONGAME_API ASerathPlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	ASerathPlayerCharacter();

	/* Returns if the serath character is in evil mode */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool InEvilMode() const { return bEvil; }

public:
	// ACharacterBase Interface ---------------
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual bool HasRequiredStats(const FAttackStats& AttackStats) const override;

public:
	virtual void OnAttackExecuted(const EAttackType Attack, const FAttackStats& AttackStats) override;
	virtual void OnAttackInterrupted(const EAttackType Attack) override;
	virtual void PostAttackEnd(const EAttackType Attack) override;
	virtual void OnAbilityTimerEnd(const EAttackType Attack) override;
	// ACharacterBase Interface --------------- End

protected:
	/* Dynamic material instance of the material used to render arms of the Serath's skeletal mesh */
	UPROPERTY(BlueprintReadWrite, Category = "Rendering")
	class UMaterialInstanceDynamic* ArmsMat_Instance;

	/* Dynamic material instance of the material used to render Chest of the Serath's skeletal mesh */
	UPROPERTY(BlueprintReadWrite, Category = "Rendering")
	class UMaterialInstanceDynamic* ChestMat_Instance;

	/* Dynamic material instance of the material used to render Torso of the Serath's skeletal mesh */
	UPROPERTY(BlueprintReadWrite, Category = "Rendering")
	class UMaterialInstanceDynamic* TorsoMat_Instance;

	/* Dynamic material instance of the material used to render Wing Webbings of the Serath's skeletal mesh */
	UPROPERTY(BlueprintReadWrite, Category = "Rendering")
	class UMaterialInstanceDynamic* WingWebbingMat_Instance;

	/* Dynamic material instance of the material used to render Wing Spline of the Serath's skeletal mesh */
	UPROPERTY(BlueprintReadWrite, Category = "Rendering")
	class UMaterialInstanceDynamic* WingSplineMat_Instance;

	/* Index for the arms material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rendering")
	int32 ArmsMatIndex;

	/* Index for the chest material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rendering")
	int32 ChestMatIndex;

	/* Index for the Torso material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rendering")
	int32 TorsoMatIndex;

	/* Index for the Wing Webbing material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rendering")
	int32 WingWebbingMatIndex;

	/* Index for the Wing Spline material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rendering")
	int32 WingSplineMatIndex;

private:
	/* If the player character is targeting ability one */
	bool bIsTargetingAbilityOne : 1;

	/* If the serath character is in evil mode */
	bool bEvil : 1;
};

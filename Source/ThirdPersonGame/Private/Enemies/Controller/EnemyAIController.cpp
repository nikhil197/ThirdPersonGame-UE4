// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Controller/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TimerManager.h"
#include "Engine.h"

#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

#include "Enemies/EnemyCharacter.h"
#include "Characters/CharacterBase.h"

float GetDistanceBetweenActors(AActor* First, AActor* Second)
{
	return FVector::Distance(First->GetActorLocation(), Second->GetActorLocation());
}

const ESoundSource::Source& GetSource(const FName& Tag)
{
	if (Tag == ESoundSource::GunFire.Name)
	{
		return ESoundSource::GunFire;
	}
	else if (Tag == ESoundSource::Movement.Name)
	{
		return ESoundSource::Movement;
	}
	else if (Tag == ESoundSource::Enviroment.Name)
	{
		return ESoundSource::Enviroment;
	}

	UE_LOG(LogTemp, Error, TEXT("Unknown Sound Source"));
	return ESoundSource::Unknown;
}

AEnemyAIController::AEnemyAIController()
{
	BehaviorComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("SoldierBehaviorTree");
	BrainComponent = BehaviorComponent;

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>("DamageSense");

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightSense");
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	SightConfig->PeripheralVisionAngleDegrees = 75;
	SightConfig->SightRadius = 2000.0f;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>("HearingSense");
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*DamageConfig);

	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdate);

	StateKeyName = FName("AIState");
	DestinationKeyName = FName("Destination");
	TargetKeyName = FName("Target");
	LastKnownLocKeyName = FName("LastKnownLocation");
	NeedAmmoKeyName = FName("NeedAmmo");
	NeedHealthKeyName = FName("NeedHealth");
	AIToFollowKeyName = FName("AIToFollow");
}

void AEnemyAIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	const EAIState CurrentState = GetAIState();
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>() && CurrentState != EAIState::FIGHTING)
	{
		const ESoundSource::Source& NewSource = GetSource(Stimulus.Tag);
		if (NewSource.Priority > 0 && (CurrentState != EAIState::FIGHTING) || (CurrentState == EAIState::INVESTIGATING && CurrentInvestigatedSource.Priority < NewSource.Priority))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Heard a noise, Investigating", true, 2.5f * FVector2D::UnitVector);
			SetDestination(Stimulus.StimulusLocation);

			SetAIState(EAIState::INVESTIGATING);

			CurrentInvestigatedSource = NewSource;

			GetWorldTimerManager().ClearTimer(TimerHandle_InvestigationTimeExpired);
			GetWorldTimerManager().SetTimer(TimerHandle_SearchTimeExpired, this, &AEnemyAIController::OnInvestigationTimeOver, InvestigationTime);
			// TODO: change the speed of the character
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		// Start targetting the Actor on if it is an enemy
		ACharacterBase* Enemy = Cast<ACharacterBase>(Actor);
		if (Enemy)
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				if (CurrentState != EAIState::SUSPICIOUS && CurrentState != EAIState::FIGHTING)
				{
					GetWorldTimerManager().SetTimer(TimerHandle_SuspicionMeterFull, FTimerDelegate::CreateUObject(this, &AEnemyAIController::OnSuspicionMeterFull, Actor), SuspicionMeterTime, false);
					SetAIState(EAIState::SUSPICIOUS);
				}
				else if (CurrentState != EAIState::FIGHTING)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Found An Enemy", true, 2.5f * FVector2D::UnitVector);
					SetTarget(Actor);
					SetAIState(EAIState::FIGHTING);
					SetLastKnownLocation(FVector::ZeroVector);
					GetWorldTimerManager().ClearTimer(TimerHandle_SearchTimeExpired);
					GetWorldTimerManager().ClearTimer(TimerHandle_InvestigationTimeExpired);
				}
			}
			else
			{
				// If Player went out of the sight but not due to AI Disengaging from the fight
				if (CurrentState == EAIState::FIGHTING && !(DoesAINeedAmmo() || DoesAINeedHealth()))
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Lost Trace, Searching For Enemy", true, 2.5f * FVector2D::UnitVector);
					SetLastKnownLocation(Stimulus.StimulusLocation);
					SetAIState(EAIState::SEARCHING);	// Start searching the enemy
					GetWorldTimerManager().SetTimer(TimerHandle_SearchTimeExpired, this, &AEnemyAIController::OnSearchTimeOver, SearchingTime);
				}
			}
		}
		else
		{
			AEnemyCharacter* Ally = Cast<AEnemyCharacter>(Actor);
			if (Ally && CurrentState == EAIState::PARTOLLING)
			{
				AEnemyAIController* fController = Cast<AEnemyAIController>(Ally->GetController());
				if (fController)
				{
					EAIState FSAIState = Ally->GetCurrentAIState();
					if (fController->GetAIState() == EAIState::FIGHTING)
					{
						// Follow the soldier
						SetAIState(EAIState::INVESTIGATING);
						GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "Found Engaged Ally", true, 2 * FVector2D::UnitVector);
						SetAIToFollow(Actor);
					}
				}

			}
		}
	}
}

void AEnemyAIController::OnPossess(class APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);

	AEnemyCharacter* SCharacter = Cast<AEnemyCharacter>(PossessedPawn);
	if (SCharacter && SCharacter->GetBehavior())
	{
		// TODO: Set the sense configurations

		if (SCharacter->GetBehavior()->BlackboardAsset)
		{
			Blackboard->InitializeBlackboard(*SCharacter->GetBehavior()->BlackboardAsset);
			SetAIState(EAIState::PARTOLLING);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Patrolling", true, 2.5f * FVector2D::UnitVector);
		}

		BehaviorComponent->StartTree(*SCharacter->GetBehavior());
	}
}

void AEnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	BehaviorComponent->StopTree();
}

void AEnemyAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	FVector FocusPoint = GetFocalPoint();

	APawn* ControlledPawn = GetPawn();
	if (!FocusPoint.IsZero() && ControlledPawn)
	{
		FVector Direction = FocusPoint - ControlledPawn->GetActorLocation();
		FRotator Rotation = Direction.Rotation();

		Rotation.Yaw = FRotator::ClampAxis(Rotation.Yaw);
		SetControlRotation(Rotation);

		if (bUpdatePawn)
		{
			ControlledPawn->FaceRotation(Rotation, DeltaTime);
		}
	}
}

void AEnemyAIController::OnSearchTimeOver()
{
	if (GetAIState() == EAIState::SEARCHING)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Exhausted Enemy Search, Going back to Patrolling", true, 2.5f * FVector2D::UnitVector);
		SetAIState(EAIState::PARTOLLING);
		SetTarget(nullptr);
		SetLastKnownLocation(FVector::ZeroVector);
	}
}

void AEnemyAIController::OnInvestigationTimeOver()
{
	if (GetAIState() == EAIState::INVESTIGATING)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Investigated the stimulus, Found Nothing.", true, 2.5f * FVector2D::UnitVector);
		SetAIState(EAIState::PARTOLLING);
		SetDestination(FVector::ZeroVector);
		CurrentInvestigatedSource = ESoundSource::Unknown;
	}
}

void AEnemyAIController::OnSuspicionMeterFull(AActor* Target)
{
	if (GetAIState() == EAIState::SUSPICIOUS)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Suspicion meter full. Attacking player", true, 2.5f * FVector2D::UnitVector);
		SetTarget(Target);
		SetAIState(EAIState::FIGHTING);
		SetLastKnownLocation(FVector::ZeroVector);
		GetWorldTimerManager().ClearTimer(TimerHandle_SearchTimeExpired);
		GetWorldTimerManager().ClearTimer(TimerHandle_InvestigationTimeExpired);
	}
}

void AEnemyAIController::SetAIState(EAIState NewState)
{
	if (Blackboard)
	{
		Blackboard->SetValueAsEnum(StateKeyName, (uint8)NewState);
	}
}

EAIState AEnemyAIController::GetAIState() const
{
	if (Blackboard)
	{
		EAIState CurrentState = (EAIState)(Blackboard->GetValueAsEnum(StateKeyName));
		return CurrentState;
	}

	return EAIState::INVALID;
}

void AEnemyAIController::SetDestination(const FVector& Destination)
{
	if (Blackboard)
	{
		if (Destination == FVector::ZeroVector)
		{
			Blackboard->ClearValue(DestinationKeyName);
			//SetFocalPoint(FAISystem::InvalidLocation, EAIFocusPriority::Gameplay);
		}
		else
		{
			Blackboard->SetValueAsVector(DestinationKeyName, Destination);
			SetFocalPoint(Destination);
		}
	}
}

FVector AEnemyAIController::GetDestination() const
{
	if (Blackboard)
	{
		return Blackboard->GetValueAsVector(DestinationKeyName);
	}

	return FVector::ZeroVector;
}

void AEnemyAIController::SetTarget(AActor* Target)
{
	if (Blackboard)
	{
		if (Target)
		{
			Blackboard->SetValueAsObject(TargetKeyName, Target);
			SetFocus(Target);
		}
		else
		{
			Blackboard->ClearValue(TargetKeyName);
			ClearFocus(EAIFocusPriority::Gameplay);
		}
	}
}

const AActor* AEnemyAIController::GetTarget() const
{
	if (Blackboard)
	{
		return Cast<AActor>(Blackboard->GetValueAsObject(TargetKeyName));
	}

	return nullptr;
}

void AEnemyAIController::SetLastKnownLocation(const FVector& Loc)
{
	if (Blackboard)
	{
		if (Loc == FVector::ZeroVector)
		{
			Blackboard->ClearValue(LastKnownLocKeyName);
			//SetFocalPoint(FAISystem::InvalidLocation, EAIFocusPriority::Gameplay);
		}
		else
		{
			Blackboard->SetValueAsVector(LastKnownLocKeyName, Loc);
			SetFocalPoint(Loc);
		}
	}
}

FVector AEnemyAIController::GetLastKnownLocation() const
{
	if (Blackboard)
	{
		return Blackboard->GetValueAsVector(LastKnownLocKeyName);
	}

	return FVector::ZeroVector;
}

bool AEnemyAIController::DoesAINeedAmmo() const
{
	if (Blackboard)
	{
		return Blackboard->GetValueAsBool(NeedAmmoKeyName);
	}

	return false;
}

bool AEnemyAIController::DoesAINeedHealth() const
{
	if (Blackboard)
	{
		return Blackboard->GetValueAsBool(NeedHealthKeyName);
	}

	return false;
}

void AEnemyAIController::SetAIToFollow(AActor* AI)
{
	if (Blackboard)
	{
		if (AI)
		{
			Blackboard->SetValueAsObject(AIToFollowKeyName, AI);
		}
		else
		{
			Blackboard->ClearValue(AIToFollowKeyName);
		}
	}
}
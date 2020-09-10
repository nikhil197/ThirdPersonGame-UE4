#pragma once

#include "GameTypes.generated.h"

/* To represent different attacks of the character */
UENUM(BlueprintType)
enum class EAttackType : uint8
{
	/* Invalid attack type (Represents no attack is in progress) */
	None,

	/* For primary attack of the character */
	Primary,

	/* For secondary attack of the character */
	Secondary,

	/* For first ability of the character */
	AbilityOne,

	/* For second ability of the character */
	AbilityTwo,

	/* For ultimate of the character */
	Ultimate
};

/* Represent the current state of the attack / ability */
UENUM(BlueprintType)
enum class EAttackState : uint8
{
	/* Attack is in idle state */
	Idle,

	/* Attack is in progress */
	Progress,

	/* Attack is in cooldown */
	Cooldown,

	/* Attack is in targeting mode (only for abilities) */
	Targeting
};

/* Represent the current state of the AI */
UENUM(BlueprintType)
enum class EAIState : uint8
{
	INVALID = 0,

	/* AI is patrolling */
	PARTOLLING,

	/* AI is suspicious of an enemy */
	SUSPICIOUS,

	/* AI is investigating a sound or some other stimulus */
	INVESTIGATING,

	/* AI is searching for the enemy */
	SEARCHING,

	/* AI is fighting the enemy */
	FIGHTING
};

//	For tag to differentiate sounds from different types of sources for AI
namespace ESoundSource
{
	enum Priority
	{
		// For sounds from unknown sources
		UNKNOWN = -1,

		// For sounds originating from any gun fire
		ENVIRONMENT = 0,

		// For sounds originating due to movement of player
		MOVEMENT,

		// For sounds originating due to firing of guns or other explosive sound sources
		FIRING
	};

	struct Source
	{
		FName Name;

		Priority Priority;
	};

	static const Source Unknown = { FName("Unknown"), Priority::UNKNOWN };

	static const Source GunFire = { FName("GunFire"), Priority::FIRING };

	static const Source Movement = { FName("Movement"), Priority::MOVEMENT };

	static const Source Enviroment = { FName("Enviroment"), Priority::ENVIRONMENT };
}
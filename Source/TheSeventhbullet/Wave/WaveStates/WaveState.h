#pragma once

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	None			UMETA(DisplayName = "None"),
	Begin			UMETA(DisplayName = "Begin"),
	Progress		UMETA(DisplayName = "Progress"),
	End				UMETA(DisplayName = "End"),
	Intermission	UMETA(DisplayName = "Intermission"),
	StageResult		UMETA(DisplayName = "StageResult")
};

UENUM(BlueprintType)
enum class EStageResult : uint8
{
	None		UMETA(DisplayName = "None"),
	Success		UMETA(DisplayName = "Success"),
	TimeOver	UMETA(DisplayName = "TimerOver"),
	PlayerDead	UMETA(DisplayName = "PlayerDead")
};

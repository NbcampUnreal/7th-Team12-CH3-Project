#pragma once

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	None			UMETA(DisplayName = "None"),
	Begin			UMETA(DisplayName = "Begin"),
	Progress		UMETA(DisplayName = "Progress"),
	End				UMETA(DisplayName = "End"),
	Intermission	UMETA(DisplayName = "Intermission")
};

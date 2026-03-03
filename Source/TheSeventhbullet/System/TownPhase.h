#pragma once

#include "CoreMinimal.h"
#include "TownPhase.generated.h"

UENUM(BlueprintType)
enum class ETownPhase : uint8
{
	None            UMETA(DisplayName = "None"),
	AcceptRequest   UMETA(DisplayName = "AcceptRequest"),
	RequestAccepted UMETA(DisplayName = "RequestAccepted"),
	WaitForNextDay  UMETA(DisplayName = "WaitForNextDay"),
};

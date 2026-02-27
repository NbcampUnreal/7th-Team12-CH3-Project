#pragma once

#include "CoreMinimal.h"
#include "UITags.generated.h"

UENUM(BlueprintType)
enum class EUILayer : uint8
{
	Game      UMETA(DisplayName = "Game"),
	GameMenu  UMETA(DisplayName = "GameMenu"),
	Menu      UMETA(DisplayName = "Menu"),
	Modal     UMETA(DisplayName = "Modal"),
};

namespace UITags
{
	const FName Inventory      = FName(TEXT("Inventory"));
	const FName HUD            = FName(TEXT("HUD"));
	const FName Crosshair      = FName(TEXT("Crosshair"));
	const FName EscMenu        = FName(TEXT("EscMenu"));
	const FName MainMenu       = FName(TEXT("MainMenu"));
	const FName LoadingScreen  = FName(TEXT("LoadingScreen"));
	const FName Storage        = FName(TEXT("Storage"));
	const FName Request        = FName(TEXT("Request"));
}

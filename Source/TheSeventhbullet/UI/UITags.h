////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * UITags - UI 위젯 식별용 FName 상수
 *
 * UIManager의 Tag 기반 API에서 사용.
 * UIDataAsset의 WidgetClassMap 키와 동일한 값을 사용해야 함.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"

namespace UITags
{
	const FName Inventory  = FName(TEXT("Inventory"));
	const FName HUD        = FName(TEXT("HUD"));
	const FName Crosshair  = FName(TEXT("Crosshair"));
	const FName EscMenu    = FName(TEXT("EscMenu"));
	const FName MainMenu   = FName(TEXT("MainMenu"));
}

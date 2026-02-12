#pragma once
#include "CoreMinimal.h"
#include "WeaponState.generated.h"

USTRUCT(BlueprintType)
struct FWeaponState
{
	GENERATED_BODY()
	
	// 해금여부 : Default 값은 false
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponState")
	bool bUnlock = false;
	
	// @TODO_주현 : SoulGem이 추가되면 아래와 같은 느낌으로(?) 추가될 예정.
	// TArray<USoulGem> EquippedSoulGems;
};

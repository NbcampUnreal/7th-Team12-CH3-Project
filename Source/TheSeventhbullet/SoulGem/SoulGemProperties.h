#pragma once
#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "SoulGemProperties.generated.h"

UENUM(BlueprintType)
enum class ESpecialOptions : uint8
{
	None,
	ReloadSpeed,
	DoubleShot
};

USTRUCT(BlueprintType)
struct THESEVENTHBULLET_API FSoulGemProperties
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	FText GemName;
	
	// 여기에 조합에 따른 스탯 옵션 추가. A B C타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	TArray<FStatusModifier> StatusModifiers;
	
	// D타입 옵션 : 특수옵션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	TArray<ESpecialOptions> SpecialOptions;
	
	// 참조를 하지는 않고, 어떤 재료로 조합된 것인지만 확인용.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	TArray<FName> SourceMaterialNames;
};

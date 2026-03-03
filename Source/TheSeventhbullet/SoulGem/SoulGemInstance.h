#pragma once
#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "SoulGemInstance.generated.h"

USTRUCT(BlueprintType)
struct THESEVENTHBULLET_API FSoulGemInstance
{
	GENERATED_BODY()
	
	// GemName
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	FText GemName;
	
	// 여기에 조합에 따른 스탯 옵션 추가. A B C타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	TArray<FStatusModifier> StatusModifiers;
	
	// D타입 옵션 : 특수옵션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	TArray<ESpecialOptions> SpecialOptions;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	int32 Grade = 1;
	
	// 참조를 하지는 않고, 어떤 재료로 조합된 것인지만 확인용.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoulGem")
	TArray<FName> SourceMaterialNames;
};

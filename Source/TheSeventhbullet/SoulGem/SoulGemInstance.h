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

	bool IsValid() const
	{
		return !GemName.IsEmpty();
	}

	FString ToDescriptionString() const
	{
		FString Info = FString::Printf(TEXT("등급: %d\n"), Grade);

		const UEnum* StatusEnum = StaticEnum<EStatusType>();
		for (const FStatusModifier& Mod : StatusModifiers)
		{
			FString StatusName = StatusEnum ? StatusEnum->GetDisplayNameTextByValue(static_cast<int64>(Mod.Status)).ToString() : TEXT("Unknown");
			if (Mod.CalculationMethod == EStatusCalculationMethod::AddFlat)
			{
				Info += FString::Printf(TEXT("%s +%.0f\n"), *StatusName, Mod.Value);
			}
			else
			{
				Info += FString::Printf(TEXT("%s x%.1f\n"), *StatusName, Mod.Value);
			}
		}

		const UEnum* SpecialEnum = StaticEnum<ESpecialOptions>();
		for (ESpecialOptions Opt : SpecialOptions)
		{
			if (Opt == ESpecialOptions::None) continue;
			FString OptName = SpecialEnum ? SpecialEnum->GetDisplayNameTextByValue(static_cast<int64>(Opt)).ToString() : TEXT("Unknown");
			Info += FString::Printf(TEXT("특수: %s\n"), *OptName);
		}

		return Info;
	}
};

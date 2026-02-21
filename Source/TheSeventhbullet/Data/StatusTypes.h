#pragma once
#include "CoreMinimal.h"
#include "StatusTypes.generated.h"

UENUM(BlueprintType)
enum class EStatusType : uint8
{
	HP,
	Stamina,
	ATK,
	DEF,
	CritChance,
	CritDamage
};

UENUM(BlueprintType)
enum class EStatusCalculationMethod : uint8
{
	AddFlat, // + 10
	Multiply // * 1.5 (50%)
};

USTRUCT(BlueprintType)
struct THESEVENTHBULLET_API FStatusModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStatusType Status; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStatusCalculationMethod CalculationMethod;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value = 0.0f; 
};
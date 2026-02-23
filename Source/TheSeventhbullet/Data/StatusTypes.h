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
	DodgeCost,
	MaxSpeed,
	SprintMultiplier,
	CritChance,
	CritDamage
};

UENUM(BlueprintType)
enum class EStatusCalculationMethod : uint8
{
	AddFlat, // 플랫 타입 + 10
	Multiply // 증폭 타입 * 1.5 (50%)
};

USTRUCT(BlueprintType)
struct THESEVENTHBULLET_API FStatusModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStatusType Status; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStatusCalculationMethod CalculationMethod;
	
	// 증폭류의 경우 20% = 1.2로 써야함.
	// 크리티컬도 1.0(100%), 1.2(120%)같은 형식으로
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value = 0.0f;
};
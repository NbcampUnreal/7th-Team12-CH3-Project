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

USTRUCT(BlueprintType)
struct FEnhancerStatus
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnhanceHp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnhanceDefense;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnhanceAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnhanceStamina;
	
	FEnhancerStatus()
	{
		EnhanceHp = 0;
		EnhanceDefense = 0;
		EnhanceAttack = 0;
		EnhanceStamina = 0;
	}
		
};

struct FEnhancerIncreaseStatus
{
	static constexpr int32 IncreaseHp = 100;
	static constexpr int32 IncreaseDefense = 2;
	static constexpr int32 IncreaseAttack = 5;
	static constexpr int32 IncreaseStamina = 50;
};

USTRUCT(BlueprintType)
struct FCharacterStat
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 HP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 Stamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 Attack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 Defence;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 CriticalChance;//크리 확률
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 CriticalHitChance;//크피 확률
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 Speed;
	
	//기본 생성자 (초기값 세팅)
	FCharacterStat()
	{
		HP = 100;
		Stamina = 100;
		Attack = 100;
		Defence = 10;
		CriticalChance = 15;
		CriticalHitChance = 150;
		Speed = 600.0f;
	}
	
	FCharacterStat operator+(const FCharacterStat& Other) const
	{
		FCharacterStat Result;
		Result.HP = this->HP + Other.HP;
		Result.Defence = this->Defence + Other.Defence;
		Result.Attack = this->Attack + Other.Attack;
		Result.Stamina = this->Stamina + Other.Stamina;
		return Result;
	}
	
	FCharacterStat& operator+=(const FCharacterStat& Other)
	{
		this->HP += Other.HP;
		this->Defence += Other.Defence;
		this->Attack += Other.Attack;
		this->Stamina += Other.Stamina;
		
		return *this;
	}
};

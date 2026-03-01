#pragma once
#include "CoreMinimal.h"
#include "StatusTypes.generated.h"

class UNiagaraSystem;

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
	float CriticalChance;//크리 확률
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CriticalDamage;//크피 확률
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 Speed;
	
	//기본 생성자 (초기값 세팅)
	FCharacterStat()
	{
		HP = 0;
		Stamina = 0;
		Attack = 0;
		Defence = 0;
		CriticalChance = 0.f;
		CriticalDamage = 0.f;
		Speed = 0.0f;
	}
	
	FCharacterStat operator+(const FCharacterStat& Other) const
	{
		FCharacterStat Result;
		Result.HP = this->HP + Other.HP;
		Result.Defence = this->Defence + Other.Defence;
		Result.Attack = this->Attack + Other.Attack;
		Result.Stamina = this->Stamina + Other.Stamina;
		Result.CriticalChance = this->CriticalChance + Other.CriticalChance;
		Result.CriticalDamage = this->CriticalDamage + Other.CriticalDamage;
		Result.Speed = this->Speed + Other.Speed;
		return Result;
	}
	
	FCharacterStat& operator+=(const FCharacterStat& Other)
	{
		this->HP += Other.HP;
		this->Defence += Other.Defence;
		this->Attack += Other.Attack;
		this->Stamina += Other.Stamina;
		this->CriticalChance += Other.CriticalChance;
		this->CriticalDamage += Other.CriticalDamage;
		this->Speed += Other.Speed;
		return *this;
	}
};

USTRUCT(BlueprintType)
struct FWeaponStat
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	float WeaponBaseDamage; // 무기 베이스 데미지.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	float WeaponDamageMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	float Range; // 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	int32 AmountOfPellets; // 발사체 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	float PelletSpreadRadius; // 탄 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	float IncreaseSpreadRadiusValue; // 트리거 시 탄 퍼짐 증가폭(샷건이 아닌 경우)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	float FireInterval; // 발사 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	int32 MaxAmmo; // 탄창 최대치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	float ReloadTime; // 재장전 시간
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	TObjectPtr<UNiagaraSystem> MuzzleFlashEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStat")
	TObjectPtr<UParticleSystem> ImpactEffect;
	
	//기본 생성자 (초기값 세팅)
	FWeaponStat()
	{
		WeaponBaseDamage = 0.f;
		WeaponDamageMultiplier = 1.0f;
		Range = 0.f;
		AmountOfPellets = 0;
		PelletSpreadRadius = 0.f;
		IncreaseSpreadRadiusValue = 0.f;
		FireInterval = 0.f;
		MaxAmmo = 0;
		ReloadTime = 0.f;
		
		MuzzleFlashEffect = nullptr;
		ImpactEffect = nullptr;
	}
};

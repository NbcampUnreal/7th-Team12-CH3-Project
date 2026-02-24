#pragma once
#include "CoreMinimal.h"
#include "DamageContext.generated.h"

USTRUCT()
struct FDamageContext
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<AActor> Attacker = nullptr;
	UPROPERTY()
	TObjectPtr<AActor> Target = nullptr;
	UPROPERTY()
	FHitResult HitResult;

	UPROPERTY()
	float CurrentDamage = 0.0f; // 현재 실제 공격력.
	UPROPERTY()
	float CurrentCritChance = 0.0f; // 현재 크리티컬 확률
	UPROPERTY()
	float CurrentCritDamage = 0.0f; // 현재 크리티컬 데미지
	
	UPROPERTY()
	float WeaponDamage = 0.0f; // 무기 공격력	
	
	UPROPERTY()
	float StatusDamage = 0.0f; // 스탯에 기반한 캐릭터 공격력
	UPROPERTY()
	float StatusCritChance = 0.0f; // 스탯에 기반한 캐릭터 크확
	UPROPERTY()
	float StatusCritDamage = 0.0f; // 스탯에 기반한 캐릭터 크뎀
	
	UPROPERTY()
	float DamageMultiplier = 1.0f; // 밸런싱 계수 : 기본 1.0
};

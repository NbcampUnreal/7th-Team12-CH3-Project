#pragma once
#include "CoreMinimal.h"
#include "DamageContext.generated.h"


class AWeaponBase;

USTRUCT()
struct FDamageContext
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<AActor> Attacker = nullptr;
	UPROPERTY()
	TObjectPtr<AActor> Target = nullptr;
	UPROPERTY()
	TObjectPtr<AWeaponBase> Weapon = nullptr;
	UPROPERTY()
	FHitResult HitResult;
	
	UPROPERTY()
	float BaseDamage = 0.0f;
	UPROPERTY()
	float CurrentDamage = 0.0f;
};

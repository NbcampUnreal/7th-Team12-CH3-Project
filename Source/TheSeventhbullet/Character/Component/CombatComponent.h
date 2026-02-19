#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

struct FDamageContext;
class UDamageModifier;
class AWeaponBase;
class UWeaponDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	
	void InitializeWeaponData(AWeaponBase* Weapon);
	
	void StartFire();
	void StopFire();
	
	void HitScanFire();
	void ConsumeAmmo();
	
	void ApplyDamageByHit(AWeaponBase* Weapon, const FHitResult& Hit);
	void ExecutePipeline(FDamageContext& Context);
	
	void DrawFireParticles(const FHitResult& Hit);
	void SpreadBullet();
	
protected:
	virtual void BeginPlay() override;
	
private:
	
	FTimerHandle FireTimerHandle;
	bool bIsFiring = false;
	
	UPROPERTY()
	TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;
	UPROPERTY()
	TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;
	UPROPERTY()
	TArray<TObjectPtr<UDamageModifier>> DamageModifiersPipeline;
	
	float FireInterval = 0.5f;
	int32 CurrentAmmo = 0;
	int32 MaxAmmo = 0;
	float PelletSpreadRadius = 3.f; // 탄 퍼짐 정도
	float IncreaseSpreadRadiusValue = 0.5f; // 트리거 시 탄 퍼짐 증가폭(샷건이 아닌 경우)

};

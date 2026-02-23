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
	void Reload();
	void ConsumeAmmo();
	
	void ApplyDamageByHit(AWeaponBase* Weapon, const FHitResult& Hit);
	void ExecutePipeline(FDamageContext& Context);
	
	void SpawnFireParticles(const FHitResult& Hit);
	
protected:
	virtual void BeginPlay() override;
	
private:
	
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	bool bIsFiring = false;
	bool bIsReloading = false;
	
	float LastFireTime = -1.f;
	
	UPROPERTY()
	TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;
	UPROPERTY()
	TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;
	UPROPERTY()
	TArray<TObjectPtr<UDamageModifier>> DamageModifiersPipeline;
	
	float FireInterval = 0.5f; // 발사 간격
	int32 CurrentAmmo = 0; // 현재 탄창
	int32 MaxAmmo = 0; // 탄창 최대치
	float ReloadTime = 1.0f; // 재장전 시간
};

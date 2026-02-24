#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/WeaponDataAsset.h"
#include "CombatComponent.generated.h"

struct FDamageContext;
class UDamageModifier;
class AWeaponBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	
	void InitializeWeaponData(UWeaponDataAsset* Weapon);
	
	void StartFire();
	void StopFire();
	
	void HitScanFire();
	void PerformTrace(FHitResult& OutHit);
	FVector TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation);
	void SpreadBullet();
	void ResetSpreadRadius();

	void Reload();
	void ConsumeAmmo();
	
	void ApplyDamageByHit(const FHitResult& Hit);
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
	bool bDrawFireDebug = true; // 발사 디버그 표시 여부
	UPROPERTY()
	float FireDebugDuration = 0.f; // 발사 디버그 지속 시간
	UPROPERTY()
	bool bDrawDebugInfinite = false; // 발사 디버그 드로우를 영구지속할지 여부
	
	UPROPERTY()
	TArray<TObjectPtr<UDamageModifier>> DamageModifiersPipeline;
	
	UPROPERTY()
	TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;
	float WeaponBaseDamage = 1.f; // 무기 베이스 데미지.
	float Range = 850.f; // 사거리
	int32 AmountOfPellets = 1; // 발사체 갯수
	float PelletSpreadRadius = 3.f; // 탄 퍼짐 정도
	float IncreaseSpreadRadiusValue = 0.5f; // 트리거 시 탄 퍼짐 증가폭(샷건이 아닌 경우)
	float FireInterval = 0.5f; // 발사 간격
	int32 CurrentAmmo = 0; // 현재 탄창
	int32 MaxAmmo = 0; // 탄창 최대치
	float ReloadTime = 1.0f; // 재장전 시간
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/StatusTypes.h"
#include "DataAsset/WeaponDataAsset.h"
#include "CombatComponent.generated.h"

struct FDamageContext;
class AMainCharacter;
class UDamageModifier;
class AWeaponBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	
	void InitializeWeaponData(UWeaponDataAsset* Weapon);
	UFUNCTION(BlueprintCallable)
	void HandleWeaponEquipmentChanged(UWeaponDataAsset* NewWeaponData);
	
	void StartFire();
	void StopFire();
	
	void HitScanFire();
	void PerformTrace(TArray<FHitResult>& OutHits);
	FVector TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation);
	void SpreadBullet();
	void ResetSpreadRadius();

	int GetCurrentAmmo() const;
	void Reload();
	void ConsumeAmmo();
	
	void ApplyDamageByHit(const FHitResult& Hit);
	void ExecutePipeline(FDamageContext& Context);
	
	void SpawnFireParticles(const FHitResult& Hit);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMainCharacter> WeaponOwner = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UWeaponDataAsset> WeaponDataView = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FWeaponStat CurrentWeaponStatus;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UDamageModifier>> DamageModifiersPipeline; // 데미지 계산 파이프라인
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FTimerHandle FireTimerHandle;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ReloadTimerHandle;
	
	int32 CurrentAmmo = 0;
	float LastFireTime = -1.f;
	
	bool bIsFiring = false; // 발사 중인지 여부
	bool bIsReloading = false; // 재장전 중인지 여부
	
	bool bDrawFireDebug = false; // 발사 디버그 표시 여부
	float FireDebugDuration = 0.f; // 발사 디버그 지속 시간
	bool bDrawDebugInfinite = false; // 발사 디버그 드로우를 영구지속할지 여부
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/StatusTypes.h"
#include "DataAsset/WeaponDataAsset.h"
#include "CombatComponent.generated.h"

class AMainCharacter;
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
	UFUNCTION(BlueprintCallable)
	void HandleWeaponEquipmentChanged(UWeaponDataAsset* NewWeaponData);
	
	void StartFire();
	void StopFire();
	
	void HitScanFire();
	void PerformTrace(FHitResult& OutHit);
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
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category ="Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UWeaponDataAsset> WeaponDataView = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category ="Weapon", meta = (AllowPrivateAccess = "true"))
	FWeaponStat CurrentWeaponStatus;
	int32 CurrentAmmo = 0;
	
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	bool bIsFiring = false;
	bool bIsReloading = false;
	float LastFireTime = -1.f;
	bool bDrawFireDebug = true; // 발사 디버그 표시 여부
	float FireDebugDuration = 5.f; // 발사 디버그 지속 시간
	bool bDrawDebugInfinite = true; // 발사 디버그 드로우를 영구지속할지 여부
	
	TArray<TObjectPtr<UDamageModifier>> DamageModifiersPipeline; // 데미지 계산 파이프라인
};

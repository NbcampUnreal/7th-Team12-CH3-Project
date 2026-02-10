#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UWeaponDataAsset;
class USphereComponent;

UCLASS()
class THESEVENTHBULLET_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
	TObjectPtr<USceneComponent> Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	void Initialize(TObjectPtr<UWeaponDataAsset> WeaponData);
	
	void StartFire();
	void StopFire();
	void Reload();
	void Fire();
	
	void ConsumeAmmo();
	FVector TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation);
	
	void Equip(TObjectPtr<AActor> NewOwner);
	void UnEquip();

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bDrawFireDebug = true; // 발사 디버그 표시 여부
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FireDebugDuration = 1.0f; // 발사 디버그 지속 시간
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bDrawDebugInfinite = false; // 발사 디버그 드로우를 영구지속할지 여부
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AActor> WeaponOwner;
	
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	float LastFireTime = -1.f;
	bool bisReloading = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponDataAsset;
	float Damage = 10.f; // 데미지 베이스
	float FireInterval = 0.5f; // 발사 간격(발사속도)
	float Range = 850.f; // 사거리
	int32 MaxAmmo = 6; // 탄창 최대치
	int32 CurrentAmmo = 0; // 현재 탄창
	float ReloadTime = 1.0f; // 재장전 시간
	int32 AmountOfPellets = 1; // 발사체 갯수
	float PelletSpreadRadius = 3.f; // 탄 퍼짐 정도
};

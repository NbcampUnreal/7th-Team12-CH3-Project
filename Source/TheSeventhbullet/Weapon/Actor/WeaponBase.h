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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AActor> WeaponOwner;
	
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	float LastFireTime = -1.f;
	bool bisReloading = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponDataAsset;
	float Damage = 10.f;
	float FireInterval = 0.5f;
	float Range = 850.f;
	int32 MaxAmmo = 6;
	int32 CurrentAmmo = 0;
	float ReloadTime = 1.0f;
	int32 AmountOfPellets = 1;
	float PelletSpreadRadius = 3.f;
};

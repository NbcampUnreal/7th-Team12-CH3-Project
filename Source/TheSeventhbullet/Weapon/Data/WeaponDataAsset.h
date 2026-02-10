#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
	None,
	Revolver,
	Rifle,
	Shotgun
};

UCLASS()
class THESEVENTHBULLET_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponTypes WeaponType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireInterval;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Range;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxReserveAmmo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	float ReloadTime = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 PelletsCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	float SpreadRadius;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	TSoftObjectPtr<UStaticMesh> MeshComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
	TSoftObjectPtr<UParticleSystem> MuzzleFlashEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
	TSoftObjectPtr<UParticleSystem> ImpactEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> ReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TSoftObjectPtr<USoundBase> ShotSound;
};

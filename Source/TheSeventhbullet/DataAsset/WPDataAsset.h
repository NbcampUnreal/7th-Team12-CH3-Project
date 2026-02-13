#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Caution : WeaponBase 기반 코드들 주석 처리 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "WPDataAsset.generated.h"

//class AWeaponBase;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	AR,
	SR,
	HG,
	SG
};

UCLASS()
class THESEVENTHBULLET_API UWPDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetType GetItemType() const override
	{
		return FPrimaryAssetType("Weapon");
	}
	
#pragma region Property
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float AttackSpeed = 1.f;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxReserveAmmo = 120;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	float ReloadTime = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|HitScan")
	float MaxRange = 10000.f;
#pragma endregion
	
#pragma region Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
	TSoftObjectPtr<UNiagaraSystem> MuzzleFlashEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
	TSoftObjectPtr<UNiagaraSystem> ImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TSoftObjectPtr<USoundBase> ShotSound;
#pragma endregion
	
#pragma region Subclass
	// Animation Layer Interface
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSubclassOf<UAnimInstance> WeaponAnimLayerClass;
	// Weapon Base
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Actor")
	//TSubclassOf<AWeaponBase> WeaponActorClass;
#pragma endregion
	

};
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class AWeaponBase;

UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
	None,
	HandGun,
	AssaultRifle,
	ShotGun
};

UCLASS()
class THESEVENTHBULLET_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponBaseBPClass; // WeaponBase
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponTypes WeaponType; // 무기타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage; // 기본 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireInterval; // 발사간격(발사속도)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Range; // 사거리
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo; // 최대 탄약
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxReserveAmmo; // 최대 예비 탄약
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	float ReloadTime = 2.0f; // 재장전 시간
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	int32 PelletsCount; // 발사 투사체(총알, 펠릿) 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	float SpreadRadius = 2.0f; // 탄 퍼짐 범위 : Default = 3.0f
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	float MaxSpreadRadius = 10.0f; // 최대 탄 퍼짐 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	float IncreaseSpreadRadius = 0.5f; // 탄 퍼짐 증가폭(샷건의 경우 값을 수정해도 고정)
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	TSoftObjectPtr<UStaticMesh> Mesh; // 메쉬
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
	TSoftObjectPtr<UParticleSystem> MuzzleFlashEffect; // 발사 이펙트(총구)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
	TSoftObjectPtr<UParticleSystem> ImpactEffect; // Hit 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> AttackMontage; // 공격 애니메이션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> ReloadMontage; // 재장전 애니메이션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TSoftObjectPtr<USoundBase> ShotSound; // 발사 사운드
};

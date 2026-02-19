#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UNiagaraSystem;
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
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	
	
protected:
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	
public:
	// 무기 데이터 받아오는 구간
	void Initialize(TObjectPtr<APawn> NewOwner);
	// 재장전
	void Reload();
	// 라인트레이스로 히트스캔
	bool PerformTrace(FHitResult& OutHit);
	// 트레이스 종료지점을 랜덤으로 계산하여 탄 퍼짐을 구현.
	FVector TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation);
	
	TObjectPtr<UWeaponDataAsset> GetWeaponDataAsset() const { return WeaponDataAsset; }
	float GetBaseDamage() const { return BaseDamage; }
	
	// 테스트용 장착 로직
	void EquipWeapon(TObjectPtr<AActor> NewWeaponOwner);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bDrawFireDebug = false; // 발사 디버그 표시 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireDebugDuration = 0.f; // 발사 디버그 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bDrawDebugInfinite = false; // 발사 디버그 드로우를 영구지속할지 여부
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AActor> WeaponOwner;
	
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	float LastFireTime = -1.f;
	bool bIsReloading = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponDataAsset;
	
	float BaseDamage = 1.f;
	float FireInterval = 1.f;
	float Range = 850.f; // 사거리
	int32 AmountOfPellets = 1; // 발사체 갯수
	float PelletSpreadRadius = 3.f;

	UPROPERTY()
	UNiagaraSystem* CachedProjectileEffect; // 발사체 이펙트를 캐싱
};

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
	// 발사 시작 : 공격키를 입력하고 있는 동안 지속적으로 Fire() 함수를 호출
	void StartFire();
	// 발사 종료 : 공격키 입력 중지 시에 호출
	void StopFire();
	// 재장전
	void Reload();
	// 1발 발사
	void Fire();
	// 총알 감소
	void ConsumeAmmo();
	// 트레이스 종료지점을 랜덤으로 계산하여 탄 퍼짐을 구현.
	FVector TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation);
	
	// 테스트용 장착 로직
	void EquipWeapon(TObjectPtr<AActor> NewWeaponOwner);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bDrawFireDebug = true; // 발사 디버그 표시 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireDebugDuration = 1.0f; // 발사 디버그 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bDrawDebugInfinite = false; // 발사 디버그 드로우를 영구지속할지 여부
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AActor> WeaponOwner;
	
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	float LastFireTime = -1.f;
	bool bisReloading = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponDataAsset;
	
	float Damage = 10.f; // 최종 데미지
	float FireInterval = 0.5f; // 발사 간격(발사속도)
	float Range = 850.f; // 사거리
	int32 MaxAmmo = 6; // 탄창 최대치
	int32 CurrentAmmo = 0; // 현재 탄창
	float ReloadTime = 1.0f; // 재장전 시간
	int32 AmountOfPellets = 1; // 발사체 갯수
	float PelletSpreadRadius = 3.f; // 탄 퍼짐 정도
	float IncreaseSpreadRadiusValue = 0.5f; // 트리거 시 탄 퍼짐 증가폭(샷건이 아닌 경우)
};

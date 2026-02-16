// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TheSeventhbullet/Data/TableRowTypes.h"
#include "SoundManager.generated.h"

/**
 * @brief [ Centralized Sound Manager ]
 *
 * 데이터 테이블(DataTable)을 기반으로 사운드를 캐싱하고 재생하는 관리자 클래스입니다.
 *
 * * <b>1. 설계 특징 (Features)</b>
 * - <b>GameInstance Subsystem</b>: 레벨이 바뀌어도 파괴되지 않고 게임 내내 유지됩니다.
 * - <b>Data-Driven</b>: 코드 하드코딩 없이, `DT_Sound` 테이블의 Key(FName)만으로 사운드를 재생합니다.
 * - <b>Caching</b>: 초기화 시점에 사운드 데이터를 미리 로드하여 런타임 끊김(Hitch)을 방지합니다.
 *
 * * <b>2. 실전 사용 예시 (Usage Cases)</b>
 * @code
 * USoundManager* SoundMgr = USoundManager::Get(GetWorld());
 * if (SoundMgr)
 * {
 * // [Case 1] UI 클릭음 (2D 사운드, 거리 감쇠 없음)
 * SoundMgr->PlaySoundUI(TEXT("UI_Button_Click"));
 *
 * // [Case 2] 폭발음 (3D 사운드, 특정 위치에서 발생)
 * SoundMgr->PlaySoundAtLocation(TEXT("Grenade_Explosion"), GetActorLocation());
 *
 * // [Case 3] 총소리 (3D 사운드, 움직이는 액터/컴포넌트를 따라다님)
 * SoundMgr->PlaySoundAttached(TEXT("Weapon_Fire"), WeaponMeshComp);
 * }
 * @endcode
 */
UCLASS()
class THESEVENTHBULLET_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	
	static USoundManager* Get(const UObject* WorldContext);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable)
	void PlaySoundUI(FName SoundID);
	
	UFUNCTION(BlueprintCallable)
	void PlaySoundAtLocation(FName SoundID, FVector Location);
	
	UFUNCTION(BlueprintCallable)
	void PlaySoundAttached(FName SoundID, USceneComponent* AttachToComponent);
	
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void SetCategoryVolume(USoundClass* SoundClass, float Volume);
private:
	FSoundData* GetSoundData(FName SoundID);
	
	UPROPERTY()
	TMap<FName, FSoundData> SoundCache;
};

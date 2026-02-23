////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [LevelTransitionManager Guide]
 *
 * GameInstanceSubsystem - 맵 비동기 로딩 + 로딩 화면 관리
 *
 * 에셋 로드는 AsyncDataManager가 전담.
 * 이 매니저는 맵 로드 + 로딩 화면 표시 + 양쪽 진행률 통합만 담당.
 *
 * [사용 예시]
 *   ULevelTransitionManager* LTM = ULevelTransitionManager::Get(this);
 *
 *   // 1. 맵만 로드
 *   LTM->LoadLevel(TEXT("GameLevel"));
 *
 *   // 2. 맵 로드 + AsyncDataManager 에셋 로딩 진행률도 통합 표시
 *   //    → 먼저 AsyncDataManager에서 LoadBundles 호출 후
 *   UAsyncDataManager* ADM = UAsyncDataManager::Get(this);
 *   ADM->LoadBundles(FPrimaryAssetType("Item"), {"InGame"}, OnComplete);
 *   LTM->LoadLevel(TEXT("GameLevel"), true);  // bTrackAssets = true
 *
 * [내부 흐름]
 *   1. UIManager->ShowByTag(Loading) → 로딩 화면 표시
 *   2. LoadPackageAsync → 맵 비동기 로드
 *   3. Timer로 폴링 → 맵 진행률 + AsyncDataManager 진행률 가중치 합산
 *   4. 둘 다 완료 시 OpenLevel 호출
 *   5. 새 레벨의 GameMode::BeginPlay에서 HideByTag(Loading) 호출
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelTransitionManager.generated.h"

class ULoadingScreenWidget;

UCLASS()
class THESEVENTHBULLET_API ULevelTransitionManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static ULevelTransitionManager* Get(const UObject* WorldContextObject);

	// 맵만 로드
	void LoadLevel(FName MapName);

	// 맵 로드 + 에셋 로드 진행률 통합 과정
	void LoadLevel(FName MapName, bool bTrackAssets);

private:
	void BeginLoading(FName MapName);
	void PollLoadingProgress();
	void OnMapPackageLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result);
	void TryFinishLoading();
	void ExecuteLevelTransition();

	FName PendingMapName;
	FString PendingPackagePath;

	FTimerHandle ProgressTimerHandle;

	UPROPERTY()
	TObjectPtr<ULoadingScreenWidget> CachedLoadingWidget;

	bool bMapLoadComplete = false;

	// AsyncDataManager Flag
	bool bTrackAssetProgress = false;

	float LoadStartTime = 0.0f;
	float MinDisplayTime = 0.5f;

	bool bIsLoading = false;
};

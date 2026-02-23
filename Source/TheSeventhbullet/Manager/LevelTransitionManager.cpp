#include "LevelTransitionManager.h"
#include "UIManager.h"
#include "AsyncDataManager.h"
#include "TheSeventhbullet/UI/UITags.h"
#include "TheSeventhbullet/UI/LoadingScreenWidget.h"
#include "Kismet/GameplayStatics.h"

void ULevelTransitionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULevelTransitionManager::Deinitialize()
{
	if (UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr)
	{
		World->GetTimerManager().ClearTimer(ProgressTimerHandle);
	}

	Super::Deinitialize();
}

ULevelTransitionManager* ULevelTransitionManager::Get(const UObject* WorldContextObject)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GI->GetSubsystem<ULevelTransitionManager>();
	}
	return nullptr;
}

void ULevelTransitionManager::LoadLevel(FName MapName)
{
	if (bIsLoading) return;

	bTrackAssetProgress = false;
	BeginLoading(MapName);
}

void ULevelTransitionManager::LoadLevel(FName MapName, bool bTrackAssets)
{
	if (bIsLoading) return;

	bTrackAssetProgress = bTrackAssets;
	BeginLoading(MapName);
}

void ULevelTransitionManager::BeginLoading(FName MapName)
{
	bIsLoading = true;
	bMapLoadComplete = false;
	PendingMapName = MapName;
	PendingPackagePath = FString::Printf(TEXT("/Game/Maps/%s"), *MapName.ToString());
	LoadStartTime = FPlatformTime::Seconds();
	
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UUserWidget* Widget = UIMgr->ShowByTag(UITags::LoadingScreen, 9999);
		CachedLoadingWidget = Cast<ULoadingScreenWidget>(Widget);
		if (CachedLoadingWidget)
		{
			CachedLoadingWidget->SetProgress(0.0f);
		}
	}
	
	LoadPackageAsync(
		PendingPackagePath,
		FLoadPackageAsyncDelegate::CreateUObject(this, &ULevelTransitionManager::OnMapPackageLoaded)
	);
	
	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	if (World)
	{
		World->GetTimerManager().SetTimer(
			ProgressTimerHandle,
			this,
			&ULevelTransitionManager::PollLoadingProgress,
			0.03f,
			true
		);
	}
}

void ULevelTransitionManager::PollLoadingProgress()
{
	float MapProgress = 0.0f;
	if (bMapLoadComplete)
	{
		MapProgress = 1.0f;
	}
	else
	{
		float RawPercent = GetAsyncLoadPercentage(*PendingPackagePath);
		MapProgress = (RawPercent >= 0.0f) ? (RawPercent / 100.0f) : 0.0f;
	}

	float TotalProgress = MapProgress;
	
	if (bTrackAssetProgress)
	{
		float AssetProgress = 1.0f;
		UAsyncDataManager* ADM = UAsyncDataManager::Get(this);
		if (ADM)
		{
			AssetProgress = ADM->GetLoadingProgress();
		}
		
		TotalProgress = (MapProgress * 0.5f) + (AssetProgress * 0.5f);
	}

	TotalProgress = FMath::Clamp(TotalProgress, 0.0f, 1.0f);

	if (CachedLoadingWidget)
	{
		CachedLoadingWidget->SetProgress(TotalProgress);
	}

	// 맵 완료 상태에서 에셋도 완료됐는지 매 폴링마다 확인
	if (bMapLoadComplete)
	{
		TryFinishLoading();
	}
}

void ULevelTransitionManager::OnMapPackageLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result)
{
	if (Result != EAsyncLoadingResult::Succeeded)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelTransitionManager: Failed to load map package [%s]"), *PendingPackagePath);
		bIsLoading = false;
		return;
	}

	bMapLoadComplete = true;
	TryFinishLoading();
}

void ULevelTransitionManager::TryFinishLoading()
{
	if (!bMapLoadComplete) return;
	
	if (bTrackAssetProgress)
	{
		UAsyncDataManager* ADM = UAsyncDataManager::Get(this);
		if (ADM && ADM->GetLoadingProgress() < 1.0f)
		{
			return;
		}
	}
	
	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	if (World)
	{
		World->GetTimerManager().ClearTimer(ProgressTimerHandle);
	}

	if (CachedLoadingWidget)
	{
		CachedLoadingWidget->SetProgress(1.0f);
	}

	float ElapsedTime = FPlatformTime::Seconds() - LoadStartTime;
	float RemainingDelay = FMath::Max(0.0f, MinDisplayTime - ElapsedTime);

	if (World)
	{
		FTimerHandle DelayHandle;
		World->GetTimerManager().SetTimer(
			DelayHandle,
			this,
			&ULevelTransitionManager::ExecuteLevelTransition,
			FMath::Max(RemainingDelay, 0.01f),
			false
		);
	}
	else
	{
		ExecuteLevelTransition();
	}
}

void ULevelTransitionManager::ExecuteLevelTransition()
{
	bIsLoading = false;
	CachedLoadingWidget = nullptr;
	UGameplayStatics::OpenLevel(this, PendingMapName);
}

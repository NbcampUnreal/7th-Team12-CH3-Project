

#include "MainGameInstance.h"

#include "Data/SaveAndLoadGame.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/UIManager.h"
#include "UI/UITags.h"
#include "UI/LoadingScreenWidget.h"
#include "TheSeventhbullet/System/MainGameMode.h"

const FString UMainGameInstance::SaveSlotName = TEXT("TheSeventhBullet");
const FName UMainGameInstance::StartLevelName = FName(TEXT("L_Town"));

UMainGameInstance* UMainGameInstance::Get(const UObject* WorldContext)
{
	return Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}

void UMainGameInstance::SaveGameData()
{
	int32 UserIndex = 0;

	USaveAndLoadGame* SaveObj = Cast<USaveAndLoadGame>(UGameplayStatics::CreateSaveGameObject(USaveAndLoadGame::StaticClass()));

	if (SaveObj)
	{
		bool bIsSaved = UGameplayStatics::SaveGameToSlot(SaveObj, SaveSlotName, UserIndex);
		if (bIsSaved)
		{
			UE_LOG(LogTemp, Log, TEXT("Game Data Save Complete"));
		}
	}
}

void UMainGameInstance::LoadAsyncSaveData()
{
	int32 UserIndex = 0;

	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &UMainGameInstance::OnSaveDataLoadFinished);
	UGameplayStatics::AsyncLoadGameFromSlot(SaveSlotName, UserIndex, LoadedDelegate);
}

void UMainGameInstance::StartNewGame()
{
	bIsDataLoaded = true;
	GameStartMapLoad();
}

void UMainGameInstance::ReturnToMainMenu()
{
	SaveGameData();

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->ReturnToMainMenu();
	}
}

void UMainGameInstance::GameStartMapLoad()
{
	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		UIMgr->Close(UITags::MainMenu);
	}

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnMapLoadFinished");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = GetUniqueID();

	ShowLoadingScreen();

	UGameplayStatics::LoadStreamLevel(this, StartLevelName, true, false, LatentInfo);
}

void UMainGameInstance::OnSaveDataLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	USaveAndLoadGame* LoadedObj = Cast<USaveAndLoadGame>(LoadedGameData);
	if (!LoadedObj)
	{
		UE_LOG(LogTemp, Error, TEXT("Load failed"));
		return;
	}

	bIsDataLoaded = true;
	CheckAndStartGame();
}

void UMainGameInstance::OnMapLoadFinished()
{
	bIsMapLoaded = true;
	CheckAndStartGame();
}

void UMainGameInstance::CheckAndStartGame()
{
	if (bIsDataLoaded && bIsMapLoaded)
	{
		TargetProgress = 1.0f;

		bIsMapLoaded = false;
		bIsDataLoaded = false;
	}
}

void UMainGameInstance::ShowLoadingScreen()
{
	DisplayProgress = 0.0f;
	TargetProgress = 0.0f;

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UUserWidget* Widget = UIMgr->Open(UITags::LoadingScreen);
		CachedLoadingWidget = Cast<ULoadingScreenWidget>(Widget);
		if (CachedLoadingWidget)
		{
			CachedLoadingWidget->SetProgress(0.0f);
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ProgressTimerHandle, this,
			&UMainGameInstance::PollLoadingProgress, 0.03f, true
		);
	}
}

void UMainGameInstance::HideLoadingScreen()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProgressTimerHandle);
	}

	CachedLoadingWidget = nullptr;

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::LoadingScreen);
	}
}

bool UMainGameInstance::DoesSaveExist() const
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0);
}

void UMainGameInstance::PollLoadingProgress()
{
	if (!CachedLoadingWidget)
	{
		return;
	}
	
	float RealTarget = 0.0f;
	if (bIsDataLoaded) RealTarget += 0.5f;
	if (bIsMapLoaded) RealTarget += 0.5f;
	if (RealTarget > TargetProgress)
	{
		TargetProgress = RealTarget;
	}
	
	const float InterpSpeed = 2.0f;
	DisplayProgress = FMath::FInterpTo(DisplayProgress, TargetProgress, 0.03f, InterpSpeed);
	
	if (FMath::IsNearlyEqual(DisplayProgress, TargetProgress, 0.005f))
	{
		DisplayProgress = TargetProgress;
	}

	CachedLoadingWidget->SetProgress(DisplayProgress);
	
	if (DisplayProgress >= 1.0f)
	{
		HideLoadingScreen();

		AMainGameMode* GM = AMainGameMode::Get(this);
		if (GM)
		{
			GM->StartGamePlay();
		}
	}
}

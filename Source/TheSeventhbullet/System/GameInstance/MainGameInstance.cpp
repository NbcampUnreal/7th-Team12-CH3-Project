

#include "MainGameInstance.h"

#include "Character/Component/EquipmentComponent.h"
#include "Character/Component/StatusComponent.h"
#include "Data/SaveAndLoadGame.h"
#include "Interaction/ChestActor.h"
#include "Inventory/InventoryComponent.h"
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
	if (!SaveObj) return;
	
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if (!MainCharacter)
	{
		UE_LOG(LogTemp,Error, TEXT("AMainCharacter를 못가져옴"));
		return;
	}
	SaveObj->CharacterTotalStat = MainCharacter->GetTotalStatus();
	SaveObj->Gold = MainCharacter->GetGold();
			
	UEquipmentComponent* EquipmentComponent = MainCharacter->GetComponentByClass<UEquipmentComponent>();
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentComponent를 못 가져옴"));
		return;
	}
	SaveObj->EquippedSoulGems = EquipmentComponent->EquippedSoulGems;
			
	UStatusComponent* StatusComponent = MainCharacter->GetComponentByClass<UStatusComponent>();
	if (!StatusComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("StatusComponent를 못 가져옴"));
		return;
	}
	SaveObj->CharacterBaseStat = StatusComponent->GetCharacterBaseStatus();
	SaveObj->CharacterEnhanceStat = StatusComponent->GetCharacterEnhanceStatus();
			
	UInventoryComponent* CharacterInventoryComponent = MainCharacter->GetComponentByClass<UInventoryComponent>();
	if (!CharacterInventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Character InventoryComponent를 못 가져옴"));
		return;
	}
	SaveObj->CharacterInventoryItems = CharacterInventoryComponent->GetAllItems();
			
	AActor* FoundChest = UGameplayStatics::GetActorOfClass(this, AChestActor::StaticClass());
	AChestActor* ChestActor = Cast<AChestActor>(FoundChest);
	if (!ChestActor)
	{
		UE_LOG(LogTemp,Error, TEXT("ChestActor를 못 가져옴"));
		return;
	}
	UInventoryComponent* ChestInventoryComponent = ChestActor->GetComponentByClass<UInventoryComponent>();
	if (!ChestInventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ChestActor Inventory를 못 가져옴"));
	}
	SaveObj->ChestInventoryItems = ChestInventoryComponent->GetAllItems();
	
	SaveObj->CurrentDay = CurrentDay;
	SaveObj->TotalAttack = TotalRequestAttack;
	SaveObj->TotalHit = TotalRequestHit;
	
	bool bIsSaved = UGameplayStatics::SaveGameToSlot(SaveObj, SaveSlotName, UserIndex);
	if (bIsSaved)
	{
		UE_LOG(LogTemp, Log, TEXT("Game Data Save Complete"));	
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
	CurrentSaveData = Cast<USaveAndLoadGame>(LoadedGameData);
	if (!CurrentSaveData)
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
		if (!CurrentSaveData)
		{
			TargetProgress = 1.0f;
			bIsMapLoaded = false;
			bIsDataLoaded = false;
			return;
		}
		
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
		if (!MainCharacter)
		{
			UE_LOG(LogTemp,Error, TEXT("AMainCharacter를 못가져옴"));
			return;
		}
		MainCharacter->LoadData(CurrentSaveData->CharacterTotalStat, CurrentSaveData->Gold);
		
		UEquipmentComponent* EquipmentComponent = MainCharacter->GetComponentByClass<UEquipmentComponent>();
		if (!EquipmentComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("EquipmentComponent를 못 가져옴"));
			return;
		}
		EquipmentComponent->LoadData(CurrentSaveData->EquippedSoulGems);
			
		UStatusComponent* StatusComponent = MainCharacter->GetComponentByClass<UStatusComponent>();
		if (!StatusComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("StatusComponent를 못 가져옴"));
			return;
		}
		StatusComponent->LoadData(CurrentSaveData->CharacterBaseStat,CurrentSaveData->CharacterEnhanceStat);
			
		UInventoryComponent* CharacterInventoryComponent = MainCharacter->GetComponentByClass<UInventoryComponent>();
		if (!CharacterInventoryComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("Character InventoryComponent를 못 가져옴"));
			return;
		}
		CharacterInventoryComponent->LoadData(CurrentSaveData->CharacterInventoryItems);
			
		AActor* FoundChest = UGameplayStatics::GetActorOfClass(this, AChestActor::StaticClass());
		AChestActor* ChestActor = Cast<AChestActor>(FoundChest);
		if (!ChestActor)
		{
			UE_LOG(LogTemp,Error, TEXT("ChestActor를 못 가져옴"));
			return;
		}
		UInventoryComponent* ChestInventoryComponent = ChestActor->GetComponentByClass<UInventoryComponent>();
		if (!ChestInventoryComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("ChestActor Inventory를 못 가져옴"));
		}
		ChestInventoryComponent->LoadData(CurrentSaveData->ChestInventoryItems);
		
		CurrentDay = CurrentSaveData->CurrentDay;
		TotalRequestAttack = CurrentSaveData->TotalAttack;
		TotalRequestHit = CurrentSaveData->TotalHit;
		
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


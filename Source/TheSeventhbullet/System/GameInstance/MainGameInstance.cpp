

#include "MainGameInstance.h"

#include "LevelSequencePlayer.h"
#include "Character/Component/EquipmentComponent.h"
#include "Character/Component/StatusComponent.h"
#include "Data/SaveAndLoadGame.h"
#include "Interaction/ChestActor.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/UIManager.h"
#include "System/MonsterManagerSubSystem.h"
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

void UMainGameInstance::ResetGameData()
{
	CurrentDay = 1;
    
	AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Character) return;
    
	Character->ResetGold();
    
	UInventoryComponent* CharacterInventory = Character->GetComponentByClass<UInventoryComponent>();
	if (!CharacterInventory) return;
	CharacterInventory->ClearAllItems();
    
	UEquipmentComponent* Equipment = Character->GetComponentByClass<UEquipmentComponent>();
	if (!Equipment) return;
	Equipment->EquippedSoulGems.Empty();
	Equipment->OnGemEquipmentChanged.Broadcast();

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		bool bIsDeleted = UGameplayStatics::DeleteGameInSlot(SaveSlotName, 0);
		if (bIsDeleted)
		{
			UE_LOG(LogTemp, Log, TEXT("세이브 파일이 성공적으로 삭제되었습니다."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("세이브 파일 삭제에 실패했습니다."));
		}
	}
    
	CurrentSaveData = nullptr;
}

void UMainGameInstance::RequestBossStage(int32 InRequestID)
{
	PendingBossRequestID = InRequestID;
	ShowLoadingScreen();
	
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnBossSequenceLevelLoaded");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = GetUniqueID();
	
	UGameplayStatics::LoadStreamLevel(this, BossSequenceLevelName,true,false,LatentInfo);
	
	//현석 : 기존 타운맵 언로드
	FLatentActionInfo UnloadLatentInfo;
	UnloadLatentInfo.CallbackTarget = this;
	UnloadLatentInfo.ExecutionFunction = NAME_None;
	UnloadLatentInfo.Linkage = 1;
	UnloadLatentInfo.UUID = GetUniqueID() + 1;
    
	UGameplayStatics::UnloadStreamLevel(this, TownMapLevelName, UnloadLatentInfo, false);
}

void UMainGameInstance::OnBossSequenceLevelLoaded()
{
	HideLoadingScreen();
	PlayBossSequence();
}

void UMainGameInstance::OnBossMapLoaded()
{
	HideLoadingScreen();
	
	UMonsterManagerSubSystem* SubSystem = UMonsterManagerSubSystem::Get(this);
	if (SubSystem)
		SubSystem->CacheSpawners();
	
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;
	
	GM->SetTargetRequestID(PendingBossRequestID);
	GM->PrepareStageAndPreLoad();
}

void UMainGameInstance::OnBossSequenceFinishedDelegate()
{
	//현석 : 바인딩 해제
	if (BossSequencePlayer)
	{
		BossSequencePlayer->OnFinished.RemoveDynamic(
			this, &UMainGameInstance::OnBossSequenceFinishedDelegate);
	}
	OnBossSequenceFinished();
}

void UMainGameInstance::PlayBossSequence()
{
	//현석 : BossMeetSequence를 플레이
	FString SequencePath=TEXT("/Game/TheSeventhBullet/Blueprints/Enemy/Boss/Sequence/BossMeetSequence.BossMeetSequence");
	ULevelSequence* BossMeetSequence=Cast<ULevelSequence>(StaticLoadObject(ULevelSequence::StaticClass(),nullptr,*SequencePath));
	if (BossMeetSequence)
	{
		ALevelSequenceActor* OutActor;
		FMovieSceneSequencePlaybackSettings Settings;
		BossSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), BossMeetSequence, Settings, OutActor);
		if (BossSequencePlayer)
		{
			//현석 : 시퀀스 재생 종료를 위한 콜백함수 바인딩
			BossSequencePlayer->OnFinished.AddDynamic(this,&UMainGameInstance::OnBossSequenceFinishedDelegate);
			BossSequencePlayer->Play();
		}
	}
}

void UMainGameInstance::OnBossSequenceFinished()
{
	FLatentActionInfo UnLoadInfo;
	UGameplayStatics::UnloadStreamLevel(this,BossSequenceLevelName, UnLoadInfo, false);
	
	ShowLoadingScreen();
	
	FLatentActionInfo LatentActionInfo;
	LatentActionInfo.CallbackTarget = this;
	LatentActionInfo.ExecutionFunction = FName("OnBossMapLoaded");
	LatentActionInfo.Linkage = 0;
	LatentActionInfo.UUID = GetUniqueID()+1;
	
	UGameplayStatics::LoadStreamLevel(this, BossMapLevelName,true,false,LatentActionInfo);
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


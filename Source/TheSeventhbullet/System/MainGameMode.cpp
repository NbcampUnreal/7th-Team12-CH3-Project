#include "MainGameMode.h"

#include "MonsterManagerSubSystem.h"
#include "Character/Component/EquipmentComponent.h"
#include "DataAsset/MaterialDataAsset.h"
#include "TheSeventhbullet/Character/MainCharacter.h"
#include "TheSeventhbullet/Character/MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "GameInstance/MainGameInstance.h"
#include "TheSeventhbullet/Manager/SyncDataManager.h"
#include "TheSeventhbullet/Wave/WaveStateMachine.h"
#include "TheSeventhbullet/Manager/UIManager.h"
#include "TheSeventhbullet/UI/UITags.h"
#include "Windows/WindowsApplication.h"

AMainGameMode::AMainGameMode()
{
	DefaultPawnClass = AMainCharacter::StaticClass();
	PlayerControllerClass = AMainPlayerController::StaticClass();
}

AMainGameMode* AMainGameMode::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	
	UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	
	return World->GetAuthGameMode<AMainGameMode>();
}

//EndWaveState에서 다음 웨이브가 존재하는지 확인하는 함수
bool AMainGameMode::HasNextWave() const
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return false;
	
	if (DataManager->GetRequestData(CurrentRequestID).Waves.Num()
		<= CurrentWaveIndex)
	{
		return false;
	}
		
	return true;
}

void AMainGameMode::PrepareStageAndPreLoad()
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	
	FRequestRowData RequestData = DataManager->GetRequestData(CurrentRequestID);
	
	TMap<EMonsterType, int32> MaxMonsterRequirements;
	
	for (const FWaveRowData& Wave : RequestData.Waves)
	{
		TMap<EMonsterType,int32> CurrentWaveCounts;
		//최댓값 갱신
		for (const FWaveMonsterRowData& MonsterInfo : Wave.Monster)
		{
			EMonsterType Type = MonsterInfo.EnemyTypes;
			if (Type == EMonsterType::None) continue;
			
			int32& Count = CurrentWaveCounts.FindOrAdd(Type);
			Count += MonsterInfo.EnemyCount;
		}
		
		for (auto& Elem : CurrentWaveCounts)
		{
			int32& GlobalMax = MaxMonsterRequirements.FindOrAdd(Elem.Key);
			if (Elem.Value > GlobalMax)
			{
				GlobalMax = Elem.Value;
			}
		}
		
	}
	
	//서브시스템에 최댓값 pool 개수를 만들어서 넘김
	UMonsterManagerSubSystem* SubSystem = UMonsterManagerSubSystem::Get(this);
	if (SubSystem)
	{
		SubSystem->InitializePoolWithCounts(
			MaxMonsterRequirements, 
			FSimpleDelegate::CreateUObject(this, &AMainGameMode::OnStageReady)
		);
	}
}

void AMainGameMode::OnStageReady()
{
	UE_LOG(LogTemp, Log, TEXT("Stage Preparation Complete!"));
	
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::LoadingScreen);
	}
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return;
	
	const FRequestRowData RequestData = DataManager->GetRequestData(CurrentRequestID);
	StageTimeLimit = RequestData.StageTimeLimit;
	
	StageElapsedTime = 0.0f;
	CurrentWaveIndex = 0;
	bBossDead = false;
	
	if (WaveStateMachine)
	{
		WaveStateMachine->ChangeState(EWaveState::Begin);
	}
}

void AMainGameMode::SetupCurrentWaveData()
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return;
	
	FWaveRowData WaveData = DataManager->GetWaveData(CurrentRequestID, CurrentWaveIndex);
	const FRequestRowData RequestData = DataManager->GetRequestData(CurrentRequestID);
	SpawnInterval = RequestData.SpawnInterval;
	SpawnTimer = 0.0f;
	
	AliveMonsterCount = 0;
	SpawnQueue.Empty();
	
	for (const FWaveMonsterRowData& MonsterInfo : WaveData.Monster)
	{
		if (MonsterInfo.EnemyTypes == EMonsterType::None) continue;
		
		for (int32 i = 0 ; i < MonsterInfo.EnemyCount; i++)
		{
			SpawnQueue.Add(MonsterInfo.EnemyTypes);
		}
	}
	
	if (SpawnQueue.Num()>0)
	{
		int32 LastIndex = SpawnQueue.Num() -1;
		for (int32 i = 0 ; i <= LastIndex; i++)
		{
			int32 Index = FMath::RandRange(i,LastIndex);
			if (i != Index)
			{
				SpawnQueue.Swap(i,Index);
			}
		}
	}
}

void AMainGameMode::UpdateSpawnLogic(float DeltaTime)
{
	if (SpawnQueue.IsEmpty()) return;
	
	SpawnTimer += DeltaTime;
	
	if (SpawnTimer >= SpawnInterval)
	{
		SpawnTimer = 0.0f;
		SpawnOneMonster();
	}
}

bool AMainGameMode::IsWaveClear() const
{
	return SpawnQueue.IsEmpty() && AliveMonsterCount <= 0;
}

void AMainGameMode::OnMonsterKilled()
{
	AliveMonsterCount--;
}

void AMainGameMode::UpdateStageTimer(float DeltaTime)
{
	StageElapsedTime += DeltaTime;
}

bool AMainGameMode::IsStageTimeOver() const
{
	if (StageTimeLimit <= 0.0f) return false; // 0이면 무제한
	return StageElapsedTime >= StageTimeLimit;
}

float AMainGameMode::GetStageRemainingTime() const
{
	float Remaining = StageTimeLimit - StageElapsedTime;
	return FMath::Max(0.0f, Remaining);
}

float AMainGameMode::GetStageTimeLimit() const
{
	return StageTimeLimit;
}

void AMainGameMode::SetStageResult(EStageResult InResult)
{
	CurrentStageResult = InResult;
}

EStageResult AMainGameMode::GetStageResult() const
{
	return CurrentStageResult;
}

const TArray<FDroppedMaterialsData>& AMainGameMode::GetStageRewardItems() const
{
	return StageRewardItems;
}

void AMainGameMode::CleanupAllMonsters()
{
	SpawnQueue.Empty();
	
	UMonsterManagerSubSystem* SubSystem = UMonsterManagerSubSystem::Get(this);
	if (!SubSystem) return;
	
	// TODO : MonsterManagerSubSystem에 CleanupActivemonsters() 추가 필요
	
	AliveMonsterCount = 0;
	
}

void AMainGameMode::OnPlayerDead()
{
	CurrentStageResult = EStageResult::PlayerDead;
	if (WaveStateMachine)
	{
		WaveStateMachine->ChangeState(EWaveState::StageResult);
	}
}

void AMainGameMode::IncreaseCurrentWaveIndex()
{
	CurrentWaveIndex++;
}

float AMainGameMode::GetIntermissionDuration() const
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return 0.0f;
	
	const FRequestRowData RequestData = DataManager->GetRequestData(CurrentRequestID);
	return RequestData.IntermissionDuration;
}

void AMainGameMode::SpawnOneMonster()
{
	if (SpawnQueue.IsEmpty()) return;
	
	EMonsterType MonsterToSpawn = SpawnQueue[0];
	SpawnQueue.RemoveAt(0);
	
	UMonsterManagerSubSystem* SubSystem = UMonsterManagerSubSystem::Get(this);
	if (SubSystem)
	{
		int32 SpawnerCount = SubSystem->GetCachedSpawnerCount();
		if (SpawnerCount > 0)
		{
			int32 RandomSpawnerIndex = FMath::RandRange(0, SpawnerCount - 1);
			SubSystem->SpawnMonster(MonsterToSpawn, RandomSpawnerIndex);
			AliveMonsterCount++;
		}
	}
}

void AMainGameMode::SetTargetRequestID(int32 InRequestID)
{
	CurrentRequestID = InRequestID;
}

void AMainGameMode::ItemDropFromMonster(EMonsterType MonsterType)
{
	// 싱크매니저 가져오기
	USyncDataManager*  DataManager = USyncDataManager::Get(this);
	if (!DataManager) return;
	// Request Index 를 현재 Request Index에 따라서 보상표가 달라지기 때문에 현재 Request Index로 가져오기.
	const int32 StageIndex = CurrentRequestID;
	FMonsterDropRowData Row =  DataManager->GetDropMaterialData(MonsterType);
	if (!Row.Stages.IsValidIndex(StageIndex)) return;

	const FStageDropData& DropData = Row.Stages[StageIndex];
	if (DropData.DropRollsCount <= 0 || Row.DropEntries.Num() == 0) return;
	
	// 몬스터별 드랍 가능한 MaterialType을 결정. AllowedMaterialTypes가 비어있다면 전부 드랍하게 하면 되니까 0이어도 true
	auto IsAllowedType = [&](EMaterialTypes Type) -> bool
	{
		return (Row.AllowedMaterialTypes.Num() == 0 || Row.AllowedMaterialTypes.Contains(Type));
	};
	// Grade에 따른 가중치를 가져옴.
	auto GetGradeWeight = [&](int32 Grade) -> float
	{
		if (Grade <= 0)
		{
			return 0.0f;
		}
		const int32 Index = Grade - 1;
		
		// GradeWeight를 설정 안해두면 0으로 처리해서 해당 Grade의 아이템을 드랍하지 않음.
		return DropData.GradeWeights.IsValidIndex(Index)
			       ? DropData.GradeWeights[Index]
			       : 0.0f;
	};
	
	// 드랍풀 구조체를 안에서 선언했음. 여기서밖에 안 쓸듯?
	struct FDropPool
	{
		const FMaterialDropEntry* DropEntry = nullptr;
		float Weight = 0.0f;
	};
	
	// 이 함수를 호출하는 몬스터에게서 얻은 아이템을 저장할 배열.
	TArray<FDropPool> DropPool;
	DropPool.Reserve(Row.DropEntries.Num());
	
	for (const FMaterialDropEntry& DropEntry : Row.DropEntries)
	{
		if (!DropEntry.Material.ToSoftObjectPath().IsValid()) continue;
		
		const UMaterialDataAsset* MaterialData = DropEntry.Material.LoadSynchronous();
		if (!MaterialData) continue;
		
		const int32 Grade = MaterialData->Grade;
		if (!IsAllowedType(MaterialData->MaterialType)) continue; // 몬스터타입을 검사해서 드랍 제한.
		const float GradeWeight = GetGradeWeight(Grade); // 스테이지별 Grade에 따른 가중치를 가져옴.
		if (GradeWeight <= 0.f) continue;
		
		const float BaseWeight = FMath::Max(0.f, DropEntry.DropWeight); // 기본 가중치 
		const float FinalWeight = BaseWeight*GradeWeight; // 최종가중치 = 기본가중치*Grade에 따른 가중치
		if (FinalWeight <= 0.f) continue;
		
		DropPool.Add({ &DropEntry, FinalWeight});
	}
	
	if (DropPool.Num() == 0) return;
	
	// 이번 몬스터의 드랍결과를 임시로 저장하는 배열.
	TArray<FDroppedMaterialsData> DroppedItemFromMonster;
	
	// 드랍 롤 횟수만큼 롤을 돌린다.
	for (int32 i = 0; i <DropData.DropRollsCount; i++)
	{		
		// 아이템 드랍확률에 따라서 드랍이 발생했는지 여부를 검사. (0~1 사이의 값을 임의로 가져와서 드랍확률보다 높은지 판단하는 방식)
		if (FMath::FRand() >= DropData.DropChance) continue;
		
		float TotalWeight = 0.0f;
		for (const FDropPool& Pool : DropPool)
		{
			TotalWeight += Pool.Weight;
		}
		
		if (TotalWeight <= 0.0f) continue;
		
		float Rolling = FMath::FRandRange(0.f, TotalWeight);
		const FMaterialDropEntry* PickFromPool = nullptr;
		
		// 가중치에 따라서 
		for (const FDropPool& Pool : DropPool)
		{
			Rolling -= Pool.Weight;
			if (Rolling <= 0.f)
			{
				PickFromPool = Pool.DropEntry;
				break;
			}
		}
		
		if (!PickFromPool) continue;
		
		// 이번에 획득한 아이템을 DroppedItemFromMonster 배열에 추가.
		StackItem(DroppedItemFromMonster, PickFromPool->Material, 1);
	}
	
	// 이번에 획득한 아이템을 브로드캐스트
	OnMaterialDroppedMonsterKilled.Broadcast(DroppedItemFromMonster);
	
	// 스테이지 결과 보상 아이템 배열에 이번에 획득한 아이템을 추가해둠.
	for (const FDroppedMaterialsData& DroppedItemData : DroppedItemFromMonster)
	{
		StackItem(StageRewardItems, DroppedItemData.Material, DroppedItemData.Count);
	}
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	WaveStateMachine = NewObject<UWaveStateMachine>(this);
	WaveStateMachine->Initialize(this);

	UMonsterManagerSubSystem* SubSystem = UMonsterManagerSubSystem::Get(this);
	if (SubSystem)
	{
		if (!SubSystem->OnMonsterKilled.IsAlreadyBound(this, &AMainGameMode::OnMonsterKilled))
		{
			SubSystem->OnMonsterKilled.AddDynamic(this, &AMainGameMode::OnMonsterKilled);
		}
	}

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Open(UITags::MainMenu);
	}
}

void AMainGameMode::StartGamePlay()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::MainMenu);
	}

}

void AMainGameMode::ReturnToTown()
{
	// 웨이브 상태 리셋
	if (WaveStateMachine)
	{
		WaveStateMachine->ChangeState(EWaveState::None);
	}
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (!GI) return;
	
	GI->TotalRequestAttack += RequestAttack;
	GI->TotalRequestHit += RequestHit;
	
	RequestAttack = 0;
	RequestHit = 0;

	// 스테이지 데이터 초기화
	CurrentWaveIndex = 0;
	CurrentRequestID = INDEX_NONE;
	

	SpawnQueue.Empty();
	SpawnTimer = 0.0f;
	AliveMonsterCount = 0;
	StageElapsedTime = 0.0f;
	CurrentStageResult = EStageResult::None;
	StageRewardItems.Empty();

	// UI 정리 + 로딩 화면 표시
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::StageSuccess);
		UIMgr->Close(UITags::StageFail);
		UIMgr->Close(UITags::HUD);
		UIMgr->Close(UITags::Crosshair);
		UIMgr->Open(UITags::LoadingScreen);
	}

	// L_Town을 제외한 모든 서브레벨 언로드
	UWorld* World = GetWorld();
	if (World)
	{
		const TArray<ULevelStreaming*>& StreamingLevels = World->GetStreamingLevels();

		for (ULevelStreaming* StreamingLevel : StreamingLevels)
		{
			if (!StreamingLevel) continue;

			FString ShortName = FPackageName::GetShortName(StreamingLevel->GetWorldAssetPackageName());
			if (FName(*ShortName) == TownLevelName) continue;

			if (StreamingLevel->IsLevelLoaded())
			{
				FLatentActionInfo LatentInfo;
				UGameplayStatics::UnloadStreamLevel(this, FName(*ShortName), LatentInfo, false);
			}
		}
	}
	
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnTownLevelLoaded");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = GetUniqueID();

	UGameplayStatics::LoadStreamLevel(this, TownLevelName, true, false, LatentInfo);
}

void AMainGameMode::OnTownLevelLoaded()
{
	if (PlayerSpawnPoint.Num() > 0 && PlayerSpawnPoint[0])
	{
		APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
		if (Pawn)
		{
			Pawn->SetActorLocation(PlayerSpawnPoint[0]->GetActorLocation());
			Pawn->SetActorRotation(PlayerSpawnPoint[0]->GetActorRotation());
		}
	}

	// 로딩 화면 닫기
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::LoadingScreen);
	}
	AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Character) return;
	
	UEquipmentComponent* EquipmentComponent = Character->GetComponentByClass<UEquipmentComponent>();
	if (!EquipmentComponent) return;
	
	EquipmentComponent->CurrentWeapon = nullptr;
	
}

void AMainGameMode::ReturnToMainMenu()
{
	// 웨이브 상태 리셋
	if (WaveStateMachine)
	{
		WaveStateMachine->ChangeState(EWaveState::None);
	}
	
	RequestAttack = 0;
	RequestHit = 0;

	// 스테이지 데이터 초기화
	CurrentWaveIndex = 0;
	CurrentRequestID = INDEX_NONE;
	SpawnQueue.Empty();
	SpawnTimer = 0.0f;
	AliveMonsterCount = 0;
	SpawnTimer = 0.0f;
	StageElapsedTime = 0.0f;
	CurrentStageResult = EStageResult::None;
	StageRewardItems.Empty();

	// L_Town을 제외한 모든 서브레벨 언로드
	UWorld* World = GetWorld();
	if (World)
	{
		const FName TownName = FName(TEXT("L_Town"));
		const TArray<ULevelStreaming*>& StreamingLevels = World->GetStreamingLevels();

		for (ULevelStreaming* StreamingLevel : StreamingLevels)
		{
			if (!StreamingLevel) continue;

			FString ShortName = FPackageName::GetShortName(StreamingLevel->GetWorldAssetPackageName());
			if (FName(*ShortName) == TownName) continue;

			if (StreamingLevel->IsLevelLoaded())
			{
				FLatentActionInfo LatentInfo;
				UGameplayStatics::UnloadStreamLevel(this, FName(*ShortName), LatentInfo, false);
			}
		}
	}

	// UI 정리 후 메인메뉴 표시
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::StageSuccess);
		UIMgr->Close(UITags::StageFail);
		UIMgr->Close(UITags::HUD);
		UIMgr->Close(UITags::Crosshair);
		UIMgr->Open(UITags::MainMenu);
	}
}

void AMainGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (WaveStateMachine)
	{
		WaveStateMachine->Tick(DeltaSeconds);
	}
}

int32 AMainGameMode::GetAliveMonsterCount() const
{
	return AliveMonsterCount;
}

int32 AMainGameMode::GetCurrentRequestID() const
{
	return CurrentRequestID;
}

int32 AMainGameMode::GetCurrentWaveIndex() const
{
	return CurrentWaveIndex;
}

void AMainGameMode::StackItem(TArray<FDroppedMaterialsData>& ItemArray,
	const TSoftObjectPtr<UMaterialDataAsset> Material, int32 Count)
{
	if (!Material.ToSoftObjectPath().IsValid() || Count <= 0) return;

	const FSoftObjectPath Key = Material.ToSoftObjectPath();
	for (FDroppedMaterialsData& S : ItemArray)
	{
		if (S.Material.ToSoftObjectPath() == Key)
		{
			S.Count += Count;
			return;
		}
	}

	FDroppedMaterialsData NewStack;
	NewStack.Material = Material;
	NewStack.Count = Count;
	ItemArray.Add(NewStack);
}

void AMainGameMode::RewardsChangeBroadCasting()
{
	OnStageRewardItemsChanged.Broadcast(StageRewardItems);
}

bool AMainGameMode::HasActiveRequest() const
{
	return CurrentRequestID != INDEX_NONE;
}

float AMainGameMode::GetWaveStartDelay() const
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return 0.0f;
	
	const FRequestRowData RequestData = DataManager->GetRequestData(CurrentRequestID);
	
	return RequestData.WaveStartDelay;
}

void AMainGameMode::ClearStageRewards()
{
	StageRewardItems.Reset();
}

void AMainGameMode::TriggerBossPatternSpawn(int32 PatternWaveIndex)
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return;
	
	FWaveRowData WaveData = DataManager->GetWaveData(CurrentRequestID, PatternWaveIndex);
	
	for (const FWaveMonsterRowData& MonsterInfo : WaveData.Monster)
	{
		if (MonsterInfo.EnemyTypes == EMonsterType::None) continue;
		
		for (int32 i = 0; i < MonsterInfo.EnemyCount; i++)
		{
			SpawnQueue.Add(MonsterInfo.EnemyTypes);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("[BossPattern] Wave %d 잡몹 %d마리 추가"),
		PatternWaveIndex, SpawnQueue.Num());
	
}

void AMainGameMode::NotifyBossDead()
{
	bBossDead = true;
	OnBossWaveCleared.Broadcast();
}

bool AMainGameMode::IsBossWave() const
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return false;
	
	FWaveRowData WaveData = DataManager->GetWaveData(CurrentRequestID, CurrentWaveIndex);
	return WaveData.bIsBossWave;
}

bool AMainGameMode::IsCurrentWaveManualTrigger() const
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return false;
	
	FWaveRowData WaveData = DataManager->GetWaveData(CurrentRequestID, CurrentWaveIndex);
	return WaveData.bManualTriggerOnly;
}

bool AMainGameMode::IsBossDead() const
{
	return bBossDead;
}



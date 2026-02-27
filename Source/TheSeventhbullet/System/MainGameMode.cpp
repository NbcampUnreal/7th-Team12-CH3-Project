#include "MainGameMode.h"

#include "MonsterManagerSubSystem.h"
#include "DataAsset/MaterialDataAsset.h"
#include "TheSeventhbullet/Character/MainCharacter.h"
#include "TheSeventhbullet/Character/MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
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
	
	FRequestRowData StageData = DataManager->GetRequestData(CurrentRequestID);
	
	TMap<EMonsterType, int32> MaxMonsterRequirements;
	
	for (const FWaveRowData& Wave : StageData.Waves)
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
	
	const FRequestRowData StageData = DataManager->GetStageData(CurrentStageIndex);
	StageTimeLimit = StageData.StageTimeLimit;
	
	StageElapsedTime = 0.0f;
	CurrentWaveIndex = 0;
	
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
	const FRequestRowData StageData = DataManager->GetRequestData(CurrentRequestID);
	SpawnInterval = StageData.SpawnInterval;
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
	
	//TODO 영섭 : HUD에 남은 시간 업데이트
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
	
	const FRequestRowData StageData = DataManager->GetStageData(CurrentStageIndex);
	
	return StageData.IntermissionDuration;
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
	USyncDataManager* SDM = USyncDataManager::Get(this);
	if (!SDM) return;
	// 스테이지 인덱스를 현재 스테이지 인덱스에 따라서 보상표가 달라지기 때문에 현재 스테이지 인덱스로 가져오기.
	const int32 StageIndex = CurrentStageIndex;
	FMonsterDropRowData Row = SDM->GetDropMaterialData(MonsterType);
	if (!Row.Stages.IsValidIndex(StageIndex)) return;
	
	const FStageDropData& DropData = Row.Stages[StageIndex];
	if (DropData.DropRollsCount <= 0 || DropData.DropEntries.Num() == 0) return;
	
	// 이 함수를 호출하는 몬스터에게서 얻은 아이템을 저장할 배열.
	TArray<FDroppedMaterialsData> DroppedItemFromMonster;
	
	// 드랍 롤 횟수만큼 롤을 돌린다.
	for (int32 i = 0; i <DropData.DropRollsCount; i++)
	{
		// 아이템 드랍확률에 따라서 드랍이 발생했는지 여부를 검사. (0~1 사이의 값을 임의로 가져와서 드랍확률보다 높은지 판단하는 방식)
		if (FMath::FRand() > DropData.DropChance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed GetItem"));
			continue;
		}
		// if문을 통과했으면 드랍풀을 만들어서 드랍될 아이템을 결정.
		TArray<const FMaterialDropEntry*> DropPool;
		DropPool.Reserve(DropData.DropEntries.Num());
		
		// 드랍재료 후보들을 전부 풀에 집어넣음. 
		// 이 때 스테이지에 따라 최대 드랍 등급이 정해져 있으므로 최대 드랍 등급을 초과하지 않는 아이템만 풀에 추가 
		for (const FMaterialDropEntry& DropEntry : DropData.DropEntries)
		{
			if (!DropEntry.Material.ToSoftObjectPath().IsValid()) continue;
			
			const UMaterialDataAsset* MaterialData = DropEntry.Material.LoadSynchronous();
			const int32 Grade = MaterialData ? MaterialData->Grade : 1;
			
			if (Grade <= DropData.MaxAllowedGrade) DropPool.Add(&DropEntry);
		}
		if (DropPool.Num() == 0) continue;
		
		// 만들어진 풀에서 랜덤으로 하나의 아이템을 선택.
		const int32 ChosenIndex = FMath::RandRange(0, DropPool.Num()-1);
		const FMaterialDropEntry* PickFromPool = DropPool[ChosenIndex];
		if (!PickFromPool) continue;
		
		// 아이템의 최소 획득 수와 최대 획득 수 사이에서 랜덤한 개수를 획득. 기본값은 일단 1개, 1개라서 1개만 나옴.
		const int32 MinCount = FMath::Max(0, PickFromPool->MinCount);
		const int32 MaxCount = FMath::Max(MinCount, PickFromPool->MaxCount);
		const int32 Count = FMath::RandRange(MinCount, MaxCount);
		if (Count <= 0) continue;
		
		// 이번에 획득한 아이템을 DroppedItemFromMonster 배열에 추가.
		StackItem(DroppedItemFromMonster, PickFromPool->Material, Count);
		UE_LOG(LogTemp, Warning, TEXT("GetItem"));
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

void AMainGameMode::ReturnToMainMenu()
{
	// 웨이브 상태 리셋
	if (WaveStateMachine)
	{
		WaveStateMachine->ChangeState(EWaveState::None);
	}

	CurrentWaveIndex = 0;
	CurrentRequestID = INDEX_NONE;
	SpawnQueue.Empty();
	AliveMonsterCount = 0;
	SpawnTimer = 0.0f;
	StageElapsedTime = 0.0f;
	CurrentStageResult = EStageResult::None;

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

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Open(UITags::MainMenu);
	}
}

void AMainGameMode::ReturnToTown()
{
	CurrentWaveIndex = 0;
	CurrentRequestID = INDEX_NONE;

	SpawnQueue.Empty();
	SpawnTimer = 0.0f;
	AliveMonsterCount = 0;
	StageElapsedTime = 0.0f;
	CurrentStageResult = EStageResult::None;

	if (WaveStateMachine)
	{
		WaveStateMachine->ChangeState(EWaveState::None);
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
	
	const FRequestRowData StageData = DataManager->GetStageData(CurrentStageIndex);
	
	return StageData.WaveStartDelay;
}

void AMainGameMode::ClearStageRewards()
{
	StageRewardItems.Reset();
}



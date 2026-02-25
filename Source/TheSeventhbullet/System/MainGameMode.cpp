#include "MainGameMode.h"

#include "MonsterManagerSubSystem.h"
#include "TheSeventhbullet/Character/MainCharacter.h"
#include "TheSeventhbullet/Character/MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "TheSeventhbullet/Manager/SyncDataManager.h"
#include "TheSeventhbullet/Wave/WaveStateMachine.h"
#include "TheSeventhbullet/Manager/UIManager.h"
#include "TheSeventhbullet/UI/UITags.h"

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
	
	if (DataManager->GetStageData(CurrentStageIndex).Waves.Num()
		<= CurrentWaveIndex)
	{
		return false;
	}
		
	return true;
}

void AMainGameMode::PrepareStageAndPreLoad()
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	
	FRequestRowData StageData = DataManager->GetStageData(CurrentStageIndex);
	
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
			EMonsterType KeyType = Elem.Key;
			int32 Required = Elem.Value;
			
			int32& GlobalMax = MaxMonsterRequirements.FindOrAdd(KeyType);
			if (Required > GlobalMax)
			{
				GlobalMax = Required;
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
	
	if (WaveStateMachine)
	{
		WaveStateMachine->ChangeState(EWaveState::Begin);
	}
}

void AMainGameMode::SetupCurrentWaveData()
{
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return;
	
	FWaveRowData WaveData = DataManager->GetWaveData(CurrentStageIndex, CurrentWaveIndex);
	const FRequestRowData StageData = DataManager->GetStageData(CurrentStageIndex);
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
		UIMgr->ShowByTag(UITags::MainMenu);
	}
}

void AMainGameMode::StartGamePlay()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->HideByTag(UITags::MainMenu);
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
	CurrentStageIndex = 0;
	SpawnQueue.Empty();
	AliveMonsterCount = 0;
	SpawnTimer = 0.0f;

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

	// 메인 메뉴 표시
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->ShowByTag(UITags::MainMenu);
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

int32 AMainGameMode::GetCurrentWaveIndex() const
{
	return CurrentStageIndex;
}

int32 AMainGameMode::GetCurrentSpawnIndex() const
{
	return CurrentWaveIndex;
}

void AMainGameMode::IncreaseCurrentSpawnIndex()
{
	CurrentWaveIndex++;
}

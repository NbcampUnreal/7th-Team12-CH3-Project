#include "MainGameMode.h"

#include "MonsterManagerSubSystem.h"
#include "TheSeventhbullet/Character/MainCharacter.h"
#include "TheSeventhbullet/Character/MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TheSeventhbullet/Manager/SyncDataManager.h"
#include "TheSeventhbullet/Wave/WaveStateMachine.h"
#include "TheSeventhbullet/Wave/Spawn/Spawner.h"

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
	
	FStageRowData StageData = DataManager->GetStageData(CurrentStageIndex);
	
	TMap<TSoftClassPtr<AActor>, int32> MaxMonsterRequirements;
	
	for (const FWaveRowData& Wave : StageData.Waves)
	{
		TMap<TSoftClassPtr<AActor>,int32> CurrentWaveCounts;
		//혹시 한 웨이브에서 같은 타입의 몬스터가 있는 경우를 대비
		for (const FWaveMonsterRowData& MonsterInfo : Wave.Monster)
		{
			FMonsterRowData MonsterData = DataManager->GetMonsterData(MonsterInfo.EnemyTypes);
			if (MonsterData.MonsterClass.IsNull()) continue;
			
			int32& Count = CurrentWaveCounts.FindOrAdd(MonsterData.MonsterClass);
			Count+=MonsterInfo.EnemyCount;
		}
		//최댓값 갱신
		for (auto& Elem : CurrentWaveCounts)
		{
			TSoftClassPtr<AActor> KeyClass = Elem.Key;
			int32 RequiredForThisWave = Elem.Value;
			
			int32& GlobalMax = MaxMonsterRequirements.FindOrAdd(KeyClass);
			
			if (RequiredForThisWave > GlobalMax)
			{
				GlobalMax = RequiredForThisWave;
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
	const FStageRowData StageData = DataManager->GetStageData(CurrentStageIndex);
	SpawnInterval = StageData.SpawnInterval;
	SpawnTimer = 0.0f;
	
	AliveMonsterCount = 0;
	SpawnQueue.Empty();
	
	for (const FWaveMonsterRowData& MonsterInfo : WaveData.Monster)
	{
		FMonsterRowData MonsterData = DataManager->GetMonsterData(MonsterInfo.EnemyTypes);
		if (MonsterData.MonsterClass.IsNull()) continue;
		
		for (int32 i = 0 ; i < MonsterInfo.EnemyCount; i++)
		{
			SpawnQueue.Add(MonsterData.MonsterClass);
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

void AMainGameMode::LoadLevel(FName OldLevel, FName NewLevel)
{
	FLatentActionInfo LatentInfo;
	//UGameplayStatics::UnloadStreamLevel(this,OldLevel,  LatentInfo,true);
	UGameplayStatics::LoadStreamLevel(this, NewLevel,true,true, LatentInfo);
	PrepareStageAndPreLoad();
}

void AMainGameMode::SpawnOneMonster()
{
	if (SpawnQueue.IsEmpty()) return;
	
	TSoftClassPtr<AActor> MonsterToSpawn = SpawnQueue[0];
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
		SubSystem->OnMonsterKilled.AddDynamic(this, &AMainGameMode::OnMonsterKilled);
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

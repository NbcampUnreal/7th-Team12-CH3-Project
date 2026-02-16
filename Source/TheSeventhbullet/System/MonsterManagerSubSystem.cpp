#include "MonsterManagerSubSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/AsyncDataManager.h"
#include "Wave/Spawn/Spawner.h"

void UMonsterManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMonsterManagerSubSystem::Deinitialize()
{
	Super::Deinitialize();
}

UMonsterManagerSubSystem* UMonsterManagerSubSystem::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	
	UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	
	return World->GetSubsystem<UMonsterManagerSubSystem>();
}

void UMonsterManagerSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CacheSpawners();
}
void UMonsterManagerSubSystem::CacheSpawners()
{
	//맵에 있는 스포너를 Array에 담음
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ASpawner::StaticClass(),FoundActors);
	CachedSpawners.Reserve(FoundActors.Num());
	
	for (AActor* Actor:FoundActors)
	{
		if (ASpawner* Spawner = Cast<ASpawner>(Actor))
		{
			CachedSpawners.Add(Spawner);
		}
	}
}

void UMonsterManagerSubSystem::InitializePoolWithCounts(const TMap<TSoftClassPtr<AActor>, int32>& Requirements,
	FSimpleDelegate OnComplete)
{
	//요청받은 데이터 멤버변수에 백업(로딩하는 동안 사라지면 안되니까)
	PendingPoolRequirements = Requirements;
	if (Requirements.IsEmpty())
	{
		OnComplete.ExecuteIfBound();
		return;
	}
	
	//로드해야 할 에셋 경로만 추출
	TArray<FSoftObjectPath> PathsToLoad;
	for (auto& Pair : Requirements)
	{
		PathsToLoad.Add(Pair.Key.ToSoftObjectPath());
	}
	
	//AsyncDataManager를 통해 비동기 로드 시작
	UAsyncDataManager* DataManager = UAsyncDataManager::Get(this);
	if (DataManager)
	{
		//로드가 다 끝나면 OnAssetLoadedForPool 함수 콜백
		//TODO : AsyncManager에 추가되어야 할 코드
		//AsyncManager에게 이 경로들에 있는 에셋들 로딩해주고, 다 되면 OnLoaded 실행
		//DataManager->LoadAssetListByPath(PathsToLoad, OnAssetLoadedForPool);
	}
	else
	{
		OnComplete.ExecuteIfBound();
	}
	
}

int32 UMonsterManagerSubSystem::GetCachedSpawnerCount()
{
	return CachedSpawners.Num();
}

void UMonsterManagerSubSystem::SpawnMonster(TSoftClassPtr<AActor> MonsterAssetType, int32 SpawnPointIndex)
{
	UClass* MonsterClass = MonsterAssetType.Get();
	if (!MonsterClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnMonster Failed : Asset is not loaded (%s)"), *MonsterAssetType.ToString());
		return;
	}
	
	AActor* MonsterToSpawn = nullptr;
	
	if (MonsterPool.Contains(MonsterClass) && MonsterPool[MonsterClass].Monsters.Num() > 0)
	{
		MonsterToSpawn = MonsterPool[MonsterClass].Monsters.Pop();
	}
	else
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		MonsterToSpawn = GetWorld()->SpawnActor<AActor>(
		MonsterClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParameters
		);
		
		UE_LOG(LogTemp, Warning, TEXT("Pool was Empty! Created new instance of %s"), *GetNameSafe(MonsterClass));
	}
	if (MonsterToSpawn && CachedSpawners.IsValidIndex(SpawnPointIndex))
	{
		ASpawner* SelectedSpawner = CachedSpawners[SpawnPointIndex];
			
		FVector SpawnLocation = SelectedSpawner->GetRandomPointInVolume();
		FRotator SpawnRotator = SelectedSpawner->GetActorRotation();
			
		MonsterToSpawn->SetActorLocationAndRotation(SpawnLocation, SpawnRotator);
	}
	if (MonsterToSpawn)
	{
		MonsterToSpawn->SetActorHiddenInGame(false);
		MonsterToSpawn->SetActorEnableCollision(true);
		// if (AMonsterBase* Monster = Cast<AMonsterBase>(MonsterToSpawn))
		// {
		// 	//TODO : 몬스터 상태 초기화
		// }
	}
}

//몬스터 회수, 몬스터가 죽으면 Destroy() 대신 이 함수를 호출해야 함
void UMonsterManagerSubSystem::ReturnToPool(AActor* Monster)
{
	if (!Monster) return;
	
	Monster->SetActorHiddenInGame(true);
	Monster->SetActorEnableCollision(false);
	
	UClass* Class = Monster->GetClass();
	MonsterPool.FindOrAdd(Class).Monsters.Add(Monster);
	if (OnMonsterKilled.IsBound())
	{
		OnMonsterKilled.Broadcast();
	}
	
}

void UMonsterManagerSubSystem::OnAssetsLoadedForPool(FSimpleDelegate OnComplete)
{
	UE_LOG(LogTemp, Log, TEXT("Assets Loaded. Start Creating Object Pool.."));
	
	for (auto& Pair : PendingPoolRequirements)
	{
		TSoftClassPtr<AActor> SoftClass = Pair.Key;
		int32 MaxCount = Pair.Value;
		
		UClass* MonsterClass = SoftClass.Get();
		if (!MonsterClass) continue;
		
		int32 CurrentPoolSize = 0;
		if (MonsterPool.Contains(MonsterClass))
		{
			CurrentPoolSize = MonsterPool[MonsterClass].Monsters.Num();
		}
		int32 NeededCount = MaxCount-CurrentPoolSize;
		for (int32 i = 0; i < NeededCount;i++)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			AActor* NewMonster = GetWorld()->SpawnActor<AActor>(MonsterClass, FVector(0,0,-10000), FRotator::ZeroRotator, Params);
			
			ReturnToPool(NewMonster);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Pool Initialization Complete"));
	
	OnComplete.ExecuteIfBound();
}

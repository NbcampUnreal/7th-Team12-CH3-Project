#include "MonsterManagerSubSystem.h"

#include "DataAsset/EnemyDataAsset.h"
#include "DSP/LFO.h"
#include "Enemy/EnemyBase.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/AsyncDataManager.h"
#include "Manager/SyncDataManager.h"
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

void UMonsterManagerSubSystem::InitializePoolWithCounts(const TMap<EMonsterType, int32>& Requirements, FSimpleDelegate OnComplete)
{
	//요청받은 데이터 멤버변수에 백업(로딩하는 동안 사라지면 안되니까)
	UE_LOG(LogTemp,Error,TEXT("InitializePoolwithCounts"));
	PendingPoolRequirements = Requirements;
	OnPoolInitializationComplete = OnComplete;
	
	if (Requirements.IsEmpty())
	{
		OnComplete.ExecuteIfBound();
		return;
	}
	
	USyncDataManager* SyncData = USyncDataManager::Get(this);
	if (!SyncData) return;
	
	TArray<FPrimaryAssetId> AssetsToLoad;
	
	TArray<FName> BundlesToLoad;
	BundlesToLoad.Add(FName("Enemy"));
	
	for (auto& Pair : Requirements)
	{
		EMonsterType Type = Pair.Key;
		FMonsterRowData RowData = SyncData->GetMonsterData(Type);
		
		FPrimaryAssetId AssetId("Enemy", RowData.MonsterPDAId);
		if (AssetId.IsValid())
		{
			AssetsToLoad.AddUnique(AssetId);
		}
	}
	UAsyncDataManager* AsyncData = UAsyncDataManager::Get(this);
	if (AsyncData)
	{
		AsyncData->LoadAssetsByID(
			AssetsToLoad,
			BundlesToLoad,
			FOnBundleLoadComplete::CreateUObject(this, &UMonsterManagerSubSystem::OnAssetsLoadedForPool)
			);
	}
	else
	{
		OnPoolInitializationComplete.ExecuteIfBound();
	}
}

int32 UMonsterManagerSubSystem::GetCachedSpawnerCount()
{
	return CachedSpawners.Num();
}

void UMonsterManagerSubSystem::SpawnMonster(EMonsterType MonsterType, int32 SpawnPointIndex)
{
	AEnemyBase* EnemyToSpawn = nullptr;
	
	if (MonsterPool.Contains(MonsterType))
	{
		TArray<AEnemyBase*>& Pool = MonsterPool[MonsterType].Enemys;
		if (Pool.Num()>0)
		{
			EnemyToSpawn = Pool.Pop();
		}
	}
	
	if (!EnemyToSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pool Empty! Creating New %d"), (int32)MonsterType);
        
		// 동기 로드로 PDA 가져와서 생성 (최적화상 피해야 함)
		USyncDataManager* SyncData = USyncDataManager::Get(this);
		FMonsterRowData RowData = SyncData->GetMonsterData(MonsterType);
		FPrimaryAssetId AssetId("Enemy", RowData.MonsterPDAId);
		UEnemyDataAsset* PDA = UAssetManager::Get().GetPrimaryAssetObject<UEnemyDataAsset>(AssetId); // 로드 되어있다고 가정
        
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		EnemyToSpawn = GetWorld()->SpawnActor<AEnemyBase>(AEnemyBase::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        
		if (EnemyToSpawn && PDA)
		{
			EnemyToSpawn->SetupEnemy(PDA);
			EnemyToSpawn->SetMonsterType(MonsterType);
		}
	}
	
	if (EnemyToSpawn)
	{
		if (CachedSpawners.IsValidIndex(SpawnPointIndex))
		{
			ASpawner* SelectedSpawner = CachedSpawners[SpawnPointIndex];
			EnemyToSpawn->SetActorLocationAndRotation(
				SelectedSpawner->GetRandomPointInVolume(), 
				SelectedSpawner->GetActorRotation()
			);
		}
		EnemyToSpawn->ResetEnemy();
		EnemyToSpawn->SetActorHiddenInGame(false); // 끄면 숨김
		EnemyToSpawn->SetActorTickEnabled(true);   // 끄면 멈춤
		EnemyToSpawn->SetActorEnableCollision(true);
	}
	
}

//몬스터 회수, 몬스터가 죽으면 Destroy() 대신 이 함수를 호출해야 함
void UMonsterManagerSubSystem::ReturnToPool(AEnemyBase* Monster)
{
	if (!Monster) return;
	
	Monster->SetActorHiddenInGame(true);
	Monster->SetActorTickEnabled(false); 
	Monster->SetActorEnableCollision(true);
	
	EMonsterType Type = Monster->GetMonsterType();
	if (Type != EMonsterType::None)
	{
		MonsterPool.FindOrAdd(Type).Enemys.Add(Monster);
	}
	else
	{
		Monster->Destroy(); // 타입 정보 없으면 파괴
	}
	if (OnMonsterKilled.IsBound())
	{
		OnMonsterKilled.Broadcast();
	}
}

void UMonsterManagerSubSystem::OnAssetsLoadedForPool()
{
	UE_LOG(LogTemp, Log, TEXT("Assets Loaded. Start Creating Object Pool.."));
	
	USyncDataManager* SyncData = USyncDataManager::Get(this);
	UAsyncDataManager* AsyncData = UAsyncDataManager::Get(this);
	if (!SyncData || !AsyncData) return;
	
	for (auto& Pair : PendingPoolRequirements)
	{
		EMonsterType Type = Pair.Key;
		int32 MaxCount = Pair.Value;
		
		FMonsterRowData RowData = SyncData->GetMonsterData(Type);
		FPrimaryAssetId AssetId("Enemy",RowData.MonsterPDAId);
		
		UEnemyDataAsset* LoadedPDA = Cast<UEnemyDataAsset>(AsyncData->GetLoadedAsset(AssetId));
		
		if (!LoadedPDA) 
		{
			UE_LOG(LogTemp, Error, TEXT("PDA Load Failed for Type: %d"), (int32)Type);
			continue;
		}
		
		TArray<AEnemyBase*>& PoolArray = MonsterPool.FindOrAdd(Type).Enemys;
		int32 NeededCount = MaxCount - PoolArray.Num();
		
		for (int32 i = 0; i < NeededCount; i++)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// 화면 밖 생성
			AEnemyBase* NewMonster = GetWorld()->SpawnActor<AEnemyBase>(
				AEnemyBase::StaticClass(), 
				FVector(0, 0, -10000), 
				FRotator::ZeroRotator, 
				Params
			);

			if (NewMonster)
			{
				NewMonster->SetupEnemy(LoadedPDA);
				NewMonster->SetMonsterType(Type);
				ReturnToPool(NewMonster);
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Pool Initialization Complete"));
	OnPoolInitializationComplete.ExecuteIfBound();
}

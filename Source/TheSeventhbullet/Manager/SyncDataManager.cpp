// Fill out your copyright notice in the Description page of Project Settings.


#include "SyncDataManager.h"

#include "TheSeventhbullet/System/GameInstance/MainGameInstance.h"

USyncDataManager* USyncDataManager::Get(const UObject* WorldContext)
{
	UMainGameInstance* GameInstance = UMainGameInstance::Get(WorldContext);
	if (!GameInstance) return nullptr;
	
	return GameInstance->GetSubsystem<USyncDataManager>();
}

void USyncDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	LoadAndCacheTable<FRequestRowData, int32>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Wave"),
		RequestCache,
		[](const FRequestRowData* Row) { return Row->RequestID;}
	);
	
	LoadAndCacheTable<FMonsterRowData, EMonsterType>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Monster"),
		MonsterCache,
		[](const FMonsterRowData* Row) { return Row->EnemyType; }
	);

	LoadAndCacheTable<FMonsterDropRowData, EMonsterType>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_ItemDropTable"),
		ItemDropCache,
		[](const FMonsterDropRowData* Row) { return Row->MonsterType; }
	);

	LoadAndCacheTable<FMaterialRecycleRowData, int32>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_MaterialRecycleTable"),
		MaterialRecycleCache,
		[](const FMaterialRecycleRowData* Row){ return Row->Grade; }
	);
}

FRequestRowData USyncDataManager::GetRequestData(int32 RequestID) const
{
	const FRequestRowData* Found = RequestCache.Find(RequestID);
	if (!Found)
	{
		UE_LOG(LogTemp, Error, TEXT("[SyncDataManager] RequestID %d not found"), RequestID);
		return FRequestRowData();
	}
	return *Found;
}

int32 USyncDataManager::GetTotalRequestCount() const
{
	return RequestCache.Num();
}

TArray<int32> USyncDataManager::GetAllRequestIDs() const
{
	TArray<int32> Keys;
	RequestCache.GetKeys(Keys);
	return Keys;
}

FMonsterRowData USyncDataManager::GetMonsterData(const EMonsterType Tag) const
{
	const FMonsterRowData* Found = MonsterCache.Find(Tag);
	if (!Found)
	{
		return FMonsterRowData();
	}
	return *Found;
}

FWaveRowData USyncDataManager::GetWaveData(int32 RequestID, int32 WaveIndex)
{
	const FRequestRowData* Found = RequestCache.Find(RequestID);
	if (!Found || !Found->Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[SyncDataManager] GetWaveData failed - RequestID: %d, WaveIndex: %d"), RequestID, WaveIndex);
		return FWaveRowData();
	}
	return Found->Waves[WaveIndex];
}

FMonsterDropRowData USyncDataManager::GetDropMaterialData(EMonsterType MonsterType) const
{
	const FMonsterDropRowData* Found = ItemDropCache.Find(MonsterType);
	if (!Found)
	{
		return FMonsterDropRowData();
	}
	return *Found;
}

FMaterialRecycleRowData USyncDataManager::GetRecycleResultData(const int32 Grade) const
{
	const FMaterialRecycleRowData* Found = MaterialRecycleCache.Find(Grade);
	if (!Found)
	{
		return FMaterialRecycleRowData();
	}
	return *Found;
}


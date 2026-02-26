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
		StageCache,
		[](const FRequestRowData* Row) { return Row->WaveNumber;}
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
}

FRequestRowData USyncDataManager::GetStageData(int32 StageIndex) const
{
	return StageCache[StageIndex];
}

int32 USyncDataManager::GetTotalRequestCount() const
{
	return StageCache.Num();
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

FWaveRowData USyncDataManager::GetWaveData(int32 StageIndex, int32 WaveIndex)
{
	return StageCache[StageIndex].Waves[WaveIndex];
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


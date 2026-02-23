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
	
	LoadAndCacheTable<FStageRowData, int32>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Wave"),
		StageCache,
		[](const FStageRowData* Row) { return Row->WaveNumber;}
	);
	
	LoadAndCacheTable<FMonsterRowData, EMonsterType>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Monster"),
		MonsterCache,
		[](const FMonsterRowData* Row) {return Row->EnemyType;}
		);
}

FStageRowData USyncDataManager::GetStageData(int32 StageIndex) const
{
	return StageCache[StageIndex];
}

int32 USyncDataManager::GetTotalWaveCount() const
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


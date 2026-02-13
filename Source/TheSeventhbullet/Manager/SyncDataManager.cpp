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
	
	LoadAndCacheTable<FWaveRowData, int32>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Wave"),
		WaveCache,
		[](const FWaveRowData* Row) { return Row->WaveNumber;}
	);
	
	LoadAndCacheTable<FMonsterRowData, FName>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Monster"),
		MonsterCache,
		[](const FMonsterRowData* Row) {return Row->EnemyType;}
		);
	
	
}

FWaveRowData USyncDataManager::GetWaveData(int32 WaveNumber) const
{
	const FWaveRowData* Found = WaveCache.Find(WaveNumber);
	if (!Found)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave %d not found"),WaveNumber);
		return FWaveRowData();
	}
	
	return *Found;
}

int32 USyncDataManager::GetTotalWaveCount() const
{
	return WaveCache.Num();
}

FMonsterRowData USyncDataManager::GetMonsterData(FName Tag) const
{
	const FMonsterRowData* Found = MonsterCache.Find(Tag);
	if (!Found)
	{
		return FMonsterRowData();
	}
	return *Found;
}


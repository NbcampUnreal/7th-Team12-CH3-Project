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
	
	WaveTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Wave")
	);
	if (!WaveTable)
	{
		UE_LOG(LogTemp, Error, TEXT("SyncDataManager : WaveTable 로드 실패!"));
		return;
	}
	TArray<FWaveRowData*> AllRows;
	WaveTable->GetAllRows(TEXT("WaveCache"),AllRows);
	
	for (const FWaveRowData* Row : AllRows)
	{
		if (Row)
		{
			WaveCache.Add(Row->WaveNumber, *Row);
		}
	}
	
	MonsterTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Monster")
	);
	if (!MonsterTable)
	{
		UE_LOG(LogTemp, Error, TEXT("SyncDataManager : MonsterTable 로드 실패!"));
		return;
	}
	
	TArray<FMonsterRowData*> AllMRows;
	MonsterTable->GetAllRows(TEXT("MonsterCache"),AllMRows);
	for (const FMonsterRowData* Row : AllMRows)
	{
		if (Row)
		{
			MonsterCache.Add(Row->EnemyType, *Row);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("SyncDataManager : %d Waves 로드 완료"), WaveCache.Num());
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

bool USyncDataManager::GetTableRow(UDataTable* Table, FName RowName, FTableRowBase& OutRow) const
{
	return true;
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


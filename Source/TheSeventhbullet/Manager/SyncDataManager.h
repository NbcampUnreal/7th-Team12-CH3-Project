// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TheSeventhbullet/Data/TableRowTypes.h"
#include "SyncDataManager.generated.h"

/**
 * 
 */

UCLASS()
class THESEVENTHBULLET_API USyncDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static USyncDataManager* Get(const UObject* WorldContext);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable, Category = "Data|Sync")
	FWaveRowData GetWaveData(int32 WaveNumber) const;
	
	UFUNCTION(BlueprintPure, Category = "Data|Sync")
	int32 GetTotalWaveCount() const;
	
	UFUNCTION(BlueprintCallable, Category = "Data|Sync")
	bool GetTableRow(UDataTable* Table, FName RowName, FTableRowBase& OutRow) const;
	
	UFUNCTION(BlueprintCallable, Category = "Data|Sync")
	FMonsterRowData GetMonsterData(FName Tag) const;
private:
	UPROPERTY()
	UDataTable* WaveTable = nullptr;
	UPROPERTY()
	TMap<int32, FWaveRowData> WaveCache;
	
	UPROPERTY()
	UDataTable* MonsterTable = nullptr;
	UPROPERTY()
	TMap<FName, FMonsterRowData> MonsterCache;

};

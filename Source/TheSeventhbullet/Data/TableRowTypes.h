// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TableRowTypes.generated.h"

/**
 * DataTable용 RowData 모음
 */
USTRUCT()
struct FWaveMonsterRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnemyCount = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EnemyTypes;
};

USTRUCT(BlueprintType)
struct FWaveRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	TArray<FWaveMonsterRowData> Monster;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DifficultyMultiplier = 1.f;
};

USTRUCT(BlueprintType)
struct FStageRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaveNumber = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnInterval = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnNumber = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWaveRowData> Waves;
};

USTRUCT(BlueprintType)
struct FMonsterRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EnemyType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<AActor> MonsterClass;
	
};

USTRUCT(BlueprintType)
struct FSoundData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SoundName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> SoundAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIs3D = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VolumeMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundConcurrency> ConcurrencySettings;
};


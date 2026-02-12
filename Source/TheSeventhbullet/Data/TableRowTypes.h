// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TableRowTypes.generated.h"

/**
 * DataTable용 RowData 모음
 */
USTRUCT(BlueprintType)
struct FSpawnRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnemyCount = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> EnemyTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DifficultyMultiplier = 1.f;
};

USTRUCT(BlueprintType)
struct FWaveRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaveNumber = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnInterval = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnNumber = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSpawnRowData> SpawnList;
};


USTRUCT(BlueprintType)
struct FMonsterRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EnemyType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> Monster;
	
};


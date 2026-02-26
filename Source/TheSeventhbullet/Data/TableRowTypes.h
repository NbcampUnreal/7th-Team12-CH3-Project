// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TableRowTypes.generated.h"

/**
 * DataTable용 RowData 모음
 */

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	None,
	WereWolf,
	Sparrow,
	Kwang,
	Rampage,
	Sevarog // Boss
};

USTRUCT()
struct FWaveMonsterRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnemyCount = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMonsterType EnemyTypes;
};

USTRUCT(BlueprintType)
struct FWaveRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWaveMonsterRowData> Monster;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DifficultyMultiplier = 1.f;
};

USTRUCT(BlueprintType)
struct FRequestRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RequestID = 0; // 의뢰 번호, 이 변수로 모든 상호작용, UI와 값을 주고받는 부분
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RequestName = "None";//의뢰 이름
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RequestGold = 0;//의뢰 보상
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RequestLevel = 1;//의뢰 난이도
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StageTimeLimit = 180.0f;// 초 단위, 0이면 무제한
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaveStartDelay = 3.0f;//웨이브 시작 전 딜레이
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IntermissionDuration = 10.0f;// 쉬는 시간
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnInterval = 0.5f; // 각 몬스터 스폰 딜레이
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// int32 SpawnNumber = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWaveRowData> Waves;
};

USTRUCT(BlueprintType)
struct FMonsterRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMonsterType EnemyType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MonsterPDAId;
	
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


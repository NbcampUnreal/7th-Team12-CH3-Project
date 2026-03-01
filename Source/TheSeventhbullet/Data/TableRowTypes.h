// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/MaterialDataAsset.h"
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
	Boss
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
	
	UPROPERTY(EditAnywhere, Category = "Boss")
	bool bIsBossWave = false;
	
	UPROPERTY(EditAnywhere, Category = "Boss")
	bool bManualTriggerOnly = false;
	
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

// 드랍된 Material들을 따로 저장해두기 위한 배열을 위한 구조체
USTRUCT(BlueprintType)
struct FDroppedMaterialsData
{
	GENERATED_BODY()
	
	//어떤 재료를 획득했는지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<class UMaterialDataAsset> Material;
	
	// 몇 개나 획득횄는지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count = 0;
};

USTRUCT(BlueprintType)
struct FMaterialDropEntry
{
	GENERATED_BODY()
	
	// 드랍하는 재료
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UMaterialDataAsset> Material;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = "0.0"))
	float DropWeight = 100.f;
};

USTRUCT(BlueprintType)
struct FStageDropData
{
	GENERATED_BODY()
	
	// 드랍 테이블을 몇 번 돌리는지에 대한 횟수. 사실상 아이템 최대 드랍 수.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 DropRollsCount = 1;
	// 각 롤을 돌릴 때마다 드랍확률. 기본적으로 일단 50%. 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float DropChance  = 0.5f;
	
	// Grade 1~6까지 있고, Grade에 따라서 Stage 인덱스별로 가중치를 주는 방식
	// [1.0, 1.0, 1.0, 0.2, 0.05, 0.01]처럼 Grade가 높은 아이템도 낮은 Stage에서 뜰 확률 자체는 있게 만드는게 좋다고 함.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> GradeWeights;
};

USTRUCT(BlueprintType)
struct FMonsterDropRowData : public FTableRowBase
{
	GENERATED_BODY()
	// 몬스터 타입 별로 드랍테이블을 관리.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMonsterType MonsterType;
	
	// 드랍되는 재료 후보들. (몬스터 별로 다르게 설정할 수도 있게 했음)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMaterialDropEntry> DropEntries;
	
	// 몬스터가 드랍할 수 있는 Material들을 Type별로 관리함.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EMaterialTypes> AllowedMaterialTypes;
	
	// 스테이지 별 드랍 데이터를 둬서, 스테이지에 따라 드랍되는 Material의 등급을 조금씩 높이는 방식.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FStageDropData> Stages;
};

USTRUCT(BlueprintType)
struct FMaterialRecycleRowData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Grade = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMaterialDropEntry> RecycleResultPool;
};

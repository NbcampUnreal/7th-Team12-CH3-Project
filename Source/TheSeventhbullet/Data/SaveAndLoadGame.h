// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MainCharacter.h"
#include "GameFramework/SaveGame.h"
#include "SaveAndLoadGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterSaveData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalGold = 0;
	
	//TODO : 골드로 업그레이드할 Stat 정보들은 기본 시작 스텟과 별개로 다른 변수로 저장할 것
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UpgradeAttack = 0; //골드로 업그레이드한 공격력 레벨
	
};

USTRUCT(BlueprintType)
struct FInventorySaveData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> MaterialCounts; // 객체 포인터가 아닌 이름이나 ID로 저장
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> OwnedSoulGems; // 획득한 소울젬 ID 리스트
};

UCLASS()
class THESEVENTHBULLET_API USaveAndLoadGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterSaveData CharacterSaveData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventorySaveData InventoryData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterStat CharacterStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurrentDay;//현재 날짜
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalAttack;//총을 맞힌 총횟수
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalHit;//총 공격을 당한 횟수
};

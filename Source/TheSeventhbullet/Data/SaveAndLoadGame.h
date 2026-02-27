// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MainCharacter.h"
#include "GameFramework/SaveGame.h"
#include "Inventory/ItemInstance.h"
#include "SaveAndLoadGame.generated.h"

UCLASS()
class THESEVENTHBULLET_API USaveAndLoadGame : public USaveGame
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterStat TotalStat;//Character 총 스텟
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterStat CharacterBaseStat;//Character 기본 스탯
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnhancerStatus EnhanceStat;//상점에서 구매한 스탯
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemInstance> CharacterInventoryItems;//캐릭터 인벤토리 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemInstance> ChestInventoryItems;//창고 인벤토리
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSoulGemInstance> EquippedSoulGems;//장착 중인 소울잼 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurrentDay;//현재 날짜
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalAttack;//총을 맞힌 총횟수
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalHit;//총 공격을 당한 횟수
};

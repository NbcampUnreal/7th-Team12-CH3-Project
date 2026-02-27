#pragma once

#include "CoreMinimal.h"
#include "Character/MainCharacter.h"
#include "GameFramework/SaveGame.h"
#include "Inventory/ItemInstance.h"
#include "SoulGem/SoulGemInstance.h"
#include "SaveAndLoadGame.generated.h"


UCLASS()
class THESEVENTHBULLET_API USaveAndLoadGame : public USaveGame
{
	GENERATED_BODY()
public:
	
	//MainCharacter에 있는 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterStat CharacterTotalStat;//Character 총 스텟
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Gold;
	
	//Character InventoryComponent에 있는 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemInstance> CharacterInventoryItems;//캐릭터 인벤토리 
	
	//Character EquippedComponent에 있는 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSoulGemInstance> EquippedSoulGems;//장착 중인 소울잼 
	
	//StatusComponent에 있는 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterStat CharacterBaseStat;//Character 기본 스탯
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnhancerStatus CharacterEnhanceStat;//상점에서 구매한 스탯
	
	//AChestActor에 있는 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemInstance> ChestInventoryItems;//창고 인벤토리
	
	//GameInstance에 있는 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentDay;//현재 날짜
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalAttack;//총을 맞힌 총횟수
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalHit;//총 공격을 당한 횟수
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"


class USaveGame;
/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	static UMainGameInstance* Get(const UObject* WorldContext);
	
	void SaveGameData();
	void StartAsyncLoad();// 비동기 로드 방식
	void GameStartMapLoad();
private:
	void OnAsyncLoadFinished(const FString& SlotName,const int32 UserIndex, USaveGame* LoadedGameData);
	void OnMapLoadFinished();	
private:
	bool bIsMapLoaded = false;
	bool bIsDataLoaded = false;
	
	void CheckAndStartGame();
	
};



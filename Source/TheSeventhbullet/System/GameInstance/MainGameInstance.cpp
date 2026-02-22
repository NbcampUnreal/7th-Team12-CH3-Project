// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"

#include "Data/SaveAndLoadGame.h"
#include "Kismet/GameplayStatics.h"

UMainGameInstance* UMainGameInstance::Get(const UObject* WorldContext)
{
	return Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}

void UMainGameInstance::SaveGameData()
{
	FString SaveSlotName = TEXT("TheSeventhBullet");
	int32 UserIndex = 0;
	
	USaveAndLoadGame* SaveObj = Cast<USaveAndLoadGame>(UGameplayStatics::CreateSaveGameObject(USaveAndLoadGame::StaticClass()));
	
	if (SaveObj)
	{
		//현재 게임의 데이터를 SaveObj에 기록
		//SaveObj->CharacterSaveData.TotalGold = 
		//TODO : 데이터 세이브 적용
		bool bIsSaved = UGameplayStatics::SaveGameToSlot(SaveObj,SaveSlotName, UserIndex);
		if (bIsSaved)
		{
			UE_LOG(LogTemp,Log, TEXT("Game Data Save Complete"));
		}
	}
}

void UMainGameInstance::StartAsyncLoad()
{
	FString SaveSlotName = TEXT("TheSeventhBullet");
	int32 UserIndex = 0;
	
	//callback 
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	
	//로드가 끝나면 OnAsyncLoadFinished 함수를 실행하도록 연결
	LoadedDelegate.BindUObject(this, &UMainGameInstance::OnAsyncLoadFinished);
	
	UE_LOG(LogTemp, Log, TEXT("Send Async load"));
	
	//백그라운드 스레드에 로드 작업 지시
	UGameplayStatics::AsyncLoadGameFromSlot(SaveSlotName, UserIndex, LoadedDelegate);
}

void UMainGameInstance::GameStartMapLoad()
{
	FLatentActionInfo LatentInfo;//비동기 작업의 진행 상태를 추적하기 위해 필요한 구조체
	LatentInfo.CallbackTarget = this;
		
	//로드가 끝나면 호출된 함수의 이름
	LatentInfo.ExecutionFunction = FName("OnMapLoadFinished");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = GetUniqueID();
	
	UGameplayStatics::LoadStreamLevel(this, TEXT("L_Dungeon"), true, false, LatentInfo);
}

//백그라운드 로드 작업이 끝나면 자동으로 실행되는 콜백 함수
void UMainGameInstance::OnAsyncLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	USaveAndLoadGame* LoadedObj = Cast<USaveAndLoadGame>(LoadedGameData);
	if (!LoadedObj)
	{
		UE_LOG(LogTemp, Error, TEXT("Load failed"));
		return;
	}
	//TODO : 로드된 데이터를 적용
	
	bIsDataLoaded = true;
	CheckAndStartGame();
}

void UMainGameInstance::OnMapLoadFinished()
{
	bIsMapLoaded = true;
	CheckAndStartGame();
}

void UMainGameInstance::CheckAndStartGame()
{
	if (bIsDataLoaded && bIsMapLoaded)
	{
		UE_LOG(LogTemp, Log, TEXT("Map and Data Load finish"));
		
		bIsMapLoaded = false;
		bIsDataLoaded = false;
		
		//TODO : Send To UI
		//OnAllLoadingFinishedEvent.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Something.. doing"));
	}
}

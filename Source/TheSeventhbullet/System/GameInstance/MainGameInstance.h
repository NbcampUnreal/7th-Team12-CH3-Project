// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

class USaveGame;
class UUIManager;
class ULoadingScreenWidget;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [MainGameInstance Guide]
 *
 * 게임 전체 생명주기를 관리하는 GameInstance 클래스
 * 엔진 시작 시 1회 생성되며 맵 전환에도 유지됨
 *
 * [역할]
 *   1. 게임 시작 흐름 관리 (NewGame / Continue)
 *   2. 세이브/로드 처리 (비동기 로드 + 동기 저장)
 *   3. 로딩 화면 표시 및 진행률 보간
 *   4. 메인 메뉴 복귀 처리
 *
 * [게임 시작 흐름]
 *   MainMenu 표시 (MainGameMode::BeginPlay)
 *     └─ NewGame 클릭 → StartNewGame()
 *          └─ bIsDataLoaded = true (세이브 로드 건너뜀) + GameStartMapLoad()
 *     └─ Continue 클릭 → LoadAsyncSaveData() + GameStartMapLoad()
 *          └─ 세이브 로드 + 맵 스트리밍 병렬 실행
 *     └─ 둘 다 완료 → CheckAndStartGame() → 보간 100% 도달 → StartGamePlay()
 *
 * [메인 메뉴 복귀]
 *   ReturnToMainMenu()
 *     └─ 세이브 저장 → 일시정지 해제 → GameMode에 리셋 위임
 *
 * [사용 예시]
 *   UMainGameInstance* GI = UMainGameInstance::Get(this);
 *
 *   // 새 게임 시작
 *   GI->StartNewGame();
 *
 *   // 이어하기
 *   GI->LoadAsyncSaveData();
 *   GI->GameStartMapLoad();
 *
 *   // 메인 메뉴로 복귀
 *   GI->ReturnToMainMenu();
 *
 *   // 세이브 존재 여부 확인
 *   bool bHasSave = GI->DoesSaveExist();
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class THESEVENTHBULLET_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	static UMainGameInstance* Get(const UObject* WorldContext);
	int32 CurrentDay = 1;
	void SaveGameData();
	void LoadAsyncSaveData();
	void GameStartMapLoad();
	void StartNewGame();
	void ReturnToMainMenu();

	bool DoesSaveExist() const;

private:
	void OnSaveDataLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	UFUNCTION()
	void OnMapLoadFinished();
	void CheckAndStartGame();

	void ShowLoadingScreen();
	void HideLoadingScreen();
	void PollLoadingProgress();

private:
	static const FString SaveSlotName;
	static const FName StartLevelName;

	bool bIsMapLoaded = false;
	bool bIsDataLoaded = false;

	UPROPERTY()
	TObjectPtr<ULoadingScreenWidget> CachedLoadingWidget;

	FTimerHandle ProgressTimerHandle;

	float DisplayProgress = 0.0f;
	float TargetProgress = 0.0f;
};

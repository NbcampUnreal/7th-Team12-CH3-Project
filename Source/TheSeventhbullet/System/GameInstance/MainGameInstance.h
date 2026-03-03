// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/SaveAndLoadGame.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

class ULevelSequencePlayer;
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
	int32 CurrentDay = 1;//현재 날짜
	int32 TotalRequestAttack = 0;//게임에서 총 공격한 횟수
	int32 TotalRequestHit = 0;//게임에서 총 공격 당한 횟수
	void SaveGameData();
	void LoadAsyncSaveData();
	void GameStartMapLoad();
	void StartNewGame();
	void ReturnToMainMenu();

	bool DoesSaveExist() const;
	
	void ResetGameData();
	
	//Town에서 보스 의뢰 수락 시 호출
	//현석 : 블루프린트 테스트용으로 추가
	UFUNCTION(BlueprintCallable)
	void RequestBossStage(int32 InRequestID);
private:
	void OnSaveDataLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	UFUNCTION()
	void OnMapLoadFinished();
	void CheckAndStartGame();

	void ShowLoadingScreen();
	void HideLoadingScreen();
	void PollLoadingProgress();
	
	UFUNCTION()
	void OnBossSequenceLevelLoaded();//시퀄스 로드 완료
	UFUNCTION()
	void OnBossMapLoaded();//보스맵 로드 완료 -> 웨이브 시작
	UFUNCTION()
	void OnBossSequenceFinishedDelegate();//시퀀스 종료 -> 보스맵 로드
	
	void PlayBossSequence();//시퀀스 재생
	void OnBossSequenceFinished();//시퀀스 종료 델리게이트 수신

private:
	static const FString SaveSlotName;
	static const FName StartLevelName;

	bool bIsMapLoaded = false;
	bool bIsDataLoaded = false;

	UPROPERTY()
	TObjectPtr<ULoadingScreenWidget> CachedLoadingWidget;

	FTimerHandle ProgressTimerHandle;
	
	UPROPERTY()
	USaveAndLoadGame* CurrentSaveData;
	
	float DisplayProgress = 0.0f;
	float TargetProgress = 0.0f;
	
	int32 PendingBossRequestID = INDEX_NONE;
	
	//현석 : SequencePlayer 캐싱 및 GC에서 메모리 해제 방지
	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> BossSequencePlayer;
	
	FName BossSequenceLevelName = FName(TEXT("L_BossSequence"));
	FName BossMapLevelName = FName(TEXT("L_Boss"));
	
	//현석 : 기존 맵 언로드를 위해 추가
	FName TownMapLevelName=FName(TEXT("L_Town"));
};

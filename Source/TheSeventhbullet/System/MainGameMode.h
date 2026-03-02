#pragma once

#include "CoreMinimal.h"
#include "Data/TableRowTypes.h"
#include "GameFramework/GameMode.h"
#include "Wave/WaveStates/WaveState.h"
#include "MainGameMode.generated.h"

class ASpawner;
class UWaveStateMachine;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaterialDroppedMonsterKilled, const TArray<FDroppedMaterialsData>&, DroppedMaterials);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageRewardItemsChanged, const TArray<FDroppedMaterialsData>&, Rewards);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossWaveStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossWaveCleared);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [MainGameMode Guide]
 *
 * Persistent Level(L_Main)의 GameMode — 웨이브 시스템 + 게임 상태 전환을 총괄
 * L_Main 하나에 GameMode가 하나이므로, 메뉴 → 게임 → 메뉴 전환을 모두 이 클래스에서 처리
 *
 * [맵 구조]
 *   L_Main (Persistent)  ← GameMode, PlayerStart 존재. 항상 로드됨
 *     ├─ L_Town (Sub)    ← 초기 맵. NewGame/Continue 시 스트리밍 로드
 *     └─ L_Dungeon (Sub) ← 전투 맵. LevelStreamTrigger로 전환
 *
 * [게임 상태 전환 흐름]
 *   BeginPlay()
 *     ├─ WaveStateMachine 생성 (None 상태)
 *     ├─ OnMonsterKilled 바인딩
 *     └─ MainMenu 표시 (UIManager)
 *
 *   StartGamePlay()  ← GameInstance에서 로딩 완료 후 호출
 *     ├─ MainMenu 숨김
 *     └─ PrepareStageAndPreLoad() → 몬스터 풀 초기화 → 웨이브 시작
 *
 *   ReturnToMainMenu()  ← EscMenu에서 BackToMenu 시 호출
 *     ├─ 웨이브 상태 리셋 (None)
 *     ├─ 스폰 데이터 초기화
 *     ├─ L_Town 제외 모든 서브레벨 언로드
 *     └─ MainMenu 표시
 *
 * [웨이브 시스템]
 *   PrepareStageAndPreLoad()  ← 스테이지 몬스터 최대 수량 계산 → Pool 예열
 *   OnStageReady()            ← Pool 준비 완료 → WaveStateMachine Begin
 *   SetupCurrentWaveData()    ← 현재 웨이브 스폰 큐 구성
 *   UpdateSpawnLogic()        ← Tick에서 SpawnInterval마다 몬스터 스폰
 *   OnMonsterKilled()         ← 남은 몬스터 카운트 감소
 *   IsWaveClear()             ← 스폰 큐 비었고 생존 몬스터 0이면 true
 *
 * [사용 예시]
 *   AMainGameMode* GM = AMainGameMode::Get(this);
 *   GM->StartGamePlay();       // 게임 시작 (GameInstance에서 호출)
 *   GM->ReturnToMainMenu();    // 메뉴 복귀 (GameInstance에서 호출)
 *   GM->IsWaveClear();         // 웨이브 클리어 확인
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 2월 27일 오후 1시 기준 
// 영섭 : 변수 이름 변경에 관해 
// *** Request : DT상에서 StageData에 들어갈 정보들을 RequestData, RequestID로 저장 
// *** Stage : MainGameMode에서 RequestData(DT상) -> StageData(GameMode내) Data 이동이 있다 
// Data와 Instance 상 이름 구분이라고 생각하면 편할듯 하다
// *** Wave : 기존 Wave와 동일
UCLASS()
class THESEVENTHBULLET_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMainGameMode();
	static AMainGameMode* Get(const UObject* WorldContext);
	
	//게임 시작 / 메뉴
	void StartGamePlay();
	void ReturnToMainMenu();//메인메뉴로 돌아가는 함수
	void ReturnToTown();//결산, 혹은 죽음 이후 마을로 돌아가는 함수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStream")
	FName TownLevelName = FName(TEXT("L_Town"));

	// Wave System
	bool HasNextWave() const;
	void PrepareStageAndPreLoad();
	void OnStageReady();
	void SetupCurrentWaveData();
	void UpdateSpawnLogic(float DeltaTime);
	bool IsWaveClear() const;
	
	UFUNCTION()
	void OnMonsterKilled();
	
	// Stage Timer
	void UpdateStageTimer(float DeltaTime);
	bool IsStageTimeOver() const;
	float GetStageRemainingTime() const;
	float GetStageTimeLimit() const;
	
	// Stage Result
	void SetStageResult(EStageResult InResult);
	EStageResult GetStageResult() const;
	const TArray<FDroppedMaterialsData>& GetStageRewardItems() const;
	void CleanupAllMonsters();
	
	//Player Dead
	void OnPlayerDead();
	
	//Wave Data Getter
	int32 GetAliveMonsterCount() const;
	int32 GetCurrentRequestID() const;
	int32 GetCurrentWaveIndex() const;
	float GetWaveStartDelay() const;
	void IncreaseCurrentWaveIndex();
	float GetIntermissionDuration() const;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PlayerSpawnPoint")
	TArray<AActor*> PlayerSpawnPoint;
	
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void SetTargetRequestID(int32 InRequestID);
	bool HasActiveRequest() const;

	// 주현 : 몬스터로부터 아이템 드랍.
	UFUNCTION()
	void ItemDropFromMonster(EMonsterType MonsterType);
	// 주현 : 스테이지 결과 보상 아이템 배열이 추가/변경 되는 경우 브로드캐스팅.
	UFUNCTION()
	void RewardsChangeBroadCasting();
	// 주현 : 스테이지 결과 보상 아이템 배열을 클리어.
	UFUNCTION()
	void ClearStageRewards();
	
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void TriggerBossPatternSpawn(int32 PatternWaveIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void NotifyBossDead();
	
	bool IsBossWave() const;
	bool IsCurrentWaveManualTrigger() const;
	bool IsBossDead() const;
	
	UPROPERTY(BlueprintAssignable)
	FOnBossWaveStarted  OnBossWaveStarted;
	
	UPROPERTY(BlueprintAssignable)
	FOnBossWaveCleared OnBossWaveCleared;
	
	int32 RequestAttack = 0;// 한 의뢰에서 총을 맞힌 횟수
	int32 RequestHit = 0;// 한 의뢰에서 공격을 당한 횟수
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void SpawnOneMonster();
private:
	UPROPERTY()
	TObjectPtr<UWaveStateMachine> WaveStateMachine;
	
	UPROPERTY()
	int32 CurrentWaveIndex = 0; // SpawnList의 인덱스 번호
	
	UPROPERTY()
	int32 CurrentRequestID = INDEX_NONE; // WaveData의 인덱스 번호 (INDEX_NONE = 의뢰 미수락)
	
	UPROPERTY()
	TArray<EMonsterType> SpawnQueue;
	
	// 주현 : 스테이지 결과 보상 아이템 배열.
	UPROPERTY()
	TArray<FDroppedMaterialsData> StageRewardItems;
	
	float SpawnTimer = 0.0f;
	float SpawnInterval = 0.0f;
	int32 AliveMonsterCount = 0;
	
	float StageTimeLimit = 0.0f;
	float StageElapsedTime = 0.0f;
	
	EStageResult CurrentStageResult = EStageResult::None;

	UFUNCTION()
	void OnTownLevelLoaded();
	
	bool bBossDead = false;
	
public:
	// 주현 : DELEGATE METHOD : 몬스터를 죽이고 아이템이 드랍된 경우
	UPROPERTY(BlueprintAssignable)
	FOnMaterialDroppedMonsterKilled OnMaterialDroppedMonsterKilled;
	// 주현 : DELEGATE METHOD : 스테이지 보상 아이템이 추가/변경 된 경우
	UPROPERTY(BlueprintAssignable)
	FOnStageRewardItemsChanged OnStageRewardItemsChanged;
	
private:
	// 주현 : ItemArray 배열에 Material을 Count개 만큼 넣어서 스택하는 함수.
	static void StackItem(TArray<FDroppedMaterialsData>& ItemArray,
	                            const TSoftObjectPtr<UMaterialDataAsset> Material,
	                            int32 Count);
};

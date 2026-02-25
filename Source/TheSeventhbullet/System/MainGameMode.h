#pragma once

#include "CoreMinimal.h"
#include "Data/TableRowTypes.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

class ASpawner;
class UWaveStateMachine;

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
UCLASS()
class THESEVENTHBULLET_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMainGameMode();
	static AMainGameMode* Get(const UObject* WorldContext);
	bool HasNextWave() const;
	void PrepareStageAndPreLoad();
	void StartGamePlay();
	//메인메뉴로 돌아가는 함수
	void ReturnToMainMenu();
	//결산, 혹은 죽음 이후 마을로 돌아가는 함수
	void ReturnToTown();
	void OnStageReady();
	void SetupCurrentWaveData();
	void UpdateSpawnLogic(float DeltaTime);
	bool IsWaveClear() const;
	
	UFUNCTION()
	void OnMonsterKilled();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PlayerSpawnPoint")
	TArray<AActor*> PlayerSpawnPoint;
	
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void SetTargetStageIndex(int32 InStageIndex);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void SpawnOneMonster();
private:
	UPROPERTY()
	TObjectPtr<UWaveStateMachine> WaveStateMachine;
	
	UPROPERTY()
	int32 CurrentWaveIndex=0; // SpawnList의 인덱스 번호
	
	UPROPERTY()
	int32 CurrentStageIndex=0; // WaveData의 인덱스 번호
	
	UPROPERTY()
	TArray<EMonsterType> SpawnQueue;
	
	float SpawnTimer = 0.0f;
	float SpawnInterval = 0.0f;
	int32 AliveMonsterCount = 0;
	
public:
	int32 GetCurrentWaveIndex() const;
	int32 GetCurrentSpawnIndex() const;
	void IncreaseCurrentSpawnIndex();
};

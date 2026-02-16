#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

class ASpawner;
class UWaveStateMachine;
/**
 * @brief 웨이브 시스템의 총괄 지휘관(Brain) 역할을 수행하는 게임 모드 클래스
 * * @details
 * <b>1. 설계 의도 및 특징</b>
 * - <b>Centralized Control (중앙 제어)</b>:
 * - 웨이브 데이터 분석, 스폰 스케줄링, 상태 전이(FSM)를 전담합니다.
 * - 실제 리소스 로딩과 객체 생성은 Subsystem에 위임하여 역할(Concerns)을 분리했습니다.<br>
 * * <b>Performance Aware (성능 고려)</b>:
 * - 스테이지 시작 전 `PrepareStageAndPreLoad()`를 통해 필요한 몬스터의 최대 수량을 미리 계산합니다.
 * - 이를 기반으로 Object Pool(MonsterManagerSubSystem)을 예열(Warm-Up)하여 런타임 프레임 드랍을 방지합니다.<br><br>
 * * <b>2. 주요 흐름 (Workflow)</b>
 * - <b>준비 (Preparation)</b>: Level Load -> Max Count 계산 -> Pool 초기화 요청
 * - <b>진행 (Execution)</b>: WaveStateMachine 가동 (Begin -> Progress -> End)
 * - <b>스폰 (Spawning)</b>: SpawnQueue에서 몬스터 추출 -> 랜덤 Spawner 선정 -> Subsystem에 배치 명령
 * - <b>관리 (Management)</b>: 몬스터 사망 이벤트 수신 -> 남은 몬스터 0일 시 웨이브 클리어<br><br>
 * * <b>3. 사용 예시 (Usage)</b>
 * @code
 * AMainGameMode* GM = AMainGameMode::Get(GetWorld());
 * if (GM)
 * {
 * // 현재 웨이브 상태 확인 혹은 강제 클리어 등의 디버깅
 * if (GM->IsWaveClear()) { ... }
 * }
 * @endcode
 */
UCLASS()
class THESEVENTHBULLET_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMainGameMode();
public:
	static AMainGameMode* Get(const UObject* WorldContext);
	bool HasNextWave() const;
	void PrepareStageAndPreLoad();
	void OnStageReady();
	void SetupCurrentWaveData();
	void UpdateSpawnLogic(float DeltaTime);
	bool IsWaveClear() const;
	UFUNCTION()
	void OnMonsterKilled();
	void LoadLevel(const FName OldLevel, const FName NewLevel);
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
	TArray<TSoftClassPtr<AActor>> SpawnQueue;
	
	float SpawnTimer = 0.0f;
	float SpawnInterval = 0.0f;
	
	int32 AliveMonsterCount = 0;
	
public:
	int32 GetCurrentWaveIndex() const;
	int32 GetCurrentSpawnIndex() const;
	void IncreaseCurrentSpawnIndex();
};

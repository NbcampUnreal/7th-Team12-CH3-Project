#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

class ASpawner;
class UWaveStateMachine;

UCLASS()
class THESEVENTHBULLET_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMainGameMode();
public:
	static AMainGameMode* Get(const UObject* WorldContext);
	void PrepareNextWave();
	void StartWaveSpawn();
	bool HasNextWave() const;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
private:
	UPROPERTY()
	TObjectPtr<UWaveStateMachine> WaveStateMachine;
	UPROPERTY(VisibleAnywhere,Category="Spawn")
	TArray<ASpawner*> Spawners;
	UPROPERTY()
	int32 CurrentSpawnIndex=0; // SpawnList의 인덱스 번호
	UPROPERTY()
	int32 CurrentWaveIndex=0; // WaveData의 인덱스 번호

public:
	int32 GetCurrentWaveIndex() const;
	int32 GetCurrentSpawnIndex() const;
	void IncreaseCurrentSpawnIndex();
};

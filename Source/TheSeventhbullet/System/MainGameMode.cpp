// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "TheSeventhbullet/Character/MainCharacter.h"
#include "TheSeventhbullet/Character/MainPlayerController.h"
#include "GameInstance/MainGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TheSeventhbullet/Manager/SoundManager.h"
#include "TheSeventhbullet/Manager/SyncDataManager.h"
#include "TheSeventhbullet/Wave/WaveStateMachine.h"
#include "TheSeventhbullet/Wave/Spawn/Spawner.h"

AMainGameMode::AMainGameMode()
{
	DefaultPawnClass = AMainCharacter::StaticClass();
	PlayerControllerClass = AMainPlayerController::StaticClass();
}

AMainGameMode* AMainGameMode::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	
	UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	
	return World->GetAuthGameMode<AMainGameMode>();
}

void AMainGameMode::PrepareNextWave()
{
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	USyncDataManager* DataManager = GI->GetSubsystem<USyncDataManager>();
	
	if (CurrentWaveIndex >= DataManager->GetTotalWaveCount()) return;
	
	FWaveRowData CurrentWaveData = DataManager->GetWaveData(CurrentWaveIndex);
	
	for (ASpawner* Spawner:Spawners)
	{
		if (CurrentWaveData.SpawnList.Num() > 0)
		{
			Spawner->InitializeSpawner(CurrentWaveData.SpawnList[CurrentSpawnIndex]);
		}
	}	
}

void AMainGameMode::StartWaveSpawn()
{
	for (ASpawner* Spawner : Spawners)
	{
		Spawner->Spawn();
	}
}

bool AMainGameMode::HasNextWave() const
{
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (!GI) return false;
	USyncDataManager* DataManager = GI->GetSubsystem<USyncDataManager>();
	if (!DataManager) return false;
	
	if (DataManager->GetWaveData(CurrentWaveIndex).SpawnList.Num()
		<= CurrentSpawnIndex)
	{
		return false;
	}
		
	return true;
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	WaveStateMachine = NewObject<UWaveStateMachine>(this);
	WaveStateMachine->Initialize(this);
	
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (!GI) return;
	USyncDataManager* DataManager = GI->GetSubsystem<USyncDataManager>();
	if (!DataManager) return;
	
	//맵에 있는 스포너를 Array에 담음
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ASpawner::StaticClass(),FoundActors);
	Spawners.Reserve(FoundActors.Num());
	
	for (AActor* Actor:FoundActors)
	{
		if (ASpawner* Spawner = Cast<ASpawner>(Actor))
		{
			Spawners.Add(Spawner);
		}
	}
	
	WaveStateMachine->ChangeState(EWaveState::Begin);
}

void AMainGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (WaveStateMachine)
	{
		WaveStateMachine->Tick(DeltaSeconds);
	}
}

int32 AMainGameMode::GetCurrentWaveIndex() const
{
	return CurrentWaveIndex;
}

int32 AMainGameMode::GetCurrentSpawnIndex() const
{
	return CurrentSpawnIndex;
}

void AMainGameMode::IncreaseCurrentSpawnIndex()
{
	CurrentSpawnIndex++;
}

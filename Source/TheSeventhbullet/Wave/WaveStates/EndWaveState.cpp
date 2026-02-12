// Fill out your copyright notice in the Description page of Project Settings.


#include "EndWaveState.h"

void UEndWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("End Wave"));
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->IncreaseCurrentSpawnIndex();
	}
	
}

void UEndWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM && GM->HasNextWave())
	{
		ChangeState(EWaveState::Intermission);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("All Waves Completed!"));
		ChangeState(EWaveState::None);
	}
	
}

void UEndWaveState::Exit()
{
	Super::Exit();
}

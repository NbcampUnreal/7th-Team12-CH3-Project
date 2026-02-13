// Fill out your copyright notice in the Description page of Project Settings.


#include "BeginWaveState.h"

void UBeginWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Begin Wave"));
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->PrepareNextWave();
	}
	
}

void UBeginWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ChangeState(EWaveState::Progress);
}

void UBeginWaveState::Exit()
{
	Super::Exit();
}

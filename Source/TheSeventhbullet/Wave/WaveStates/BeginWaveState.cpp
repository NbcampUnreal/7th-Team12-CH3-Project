// Fill out your copyright notice in the Description page of Project Settings.


#include "BeginWaveState.h"

void UBeginWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Begin Wave"));
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->SetupCurrentWaveData();
	}
	DelayTimer = 3.0f;
}

void UBeginWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DelayTimer -= DeltaTime;
	
	if (DelayTimer <= 0.0f)
	{
		ChangeState(EWaveState::Progress);
	}
}

void UBeginWaveState::Exit()
{
	Super::Exit();
}

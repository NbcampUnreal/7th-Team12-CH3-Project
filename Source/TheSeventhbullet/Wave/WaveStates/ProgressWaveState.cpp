// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressWaveState.h"

void UProgressWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Progress Wave"));
	GM = AMainGameMode::Get(this);
	if (!GM) return;
}

void UProgressWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GM->UpdateSpawnLogic(DeltaTime);
	if (GM->IsWaveClear())
	{
		ChangeState(EWaveState::End);
	}
}

void UProgressWaveState::Exit()
{
	Super::Exit();
}

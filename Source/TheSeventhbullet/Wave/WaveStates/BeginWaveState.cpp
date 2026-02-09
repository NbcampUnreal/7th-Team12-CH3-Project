// Fill out your copyright notice in the Description page of Project Settings.


#include "BeginWaveState.h"

void UBeginWaveState::Enter()
{
	Super::Enter();
	ChangeState(EWaveState::Progress);
}

void UBeginWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UBeginWaveState::Exit()
{
	Super::Exit();
}

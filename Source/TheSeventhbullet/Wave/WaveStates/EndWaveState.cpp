// Fill out your copyright notice in the Description page of Project Settings.


#include "EndWaveState.h"

void UEndWaveState::Enter()
{
	Super::Enter();
	ChangeState(EWaveState::Intermission);
}

void UEndWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UEndWaveState::Exit()
{
	Super::Exit();
}

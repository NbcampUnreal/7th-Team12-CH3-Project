// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressWaveState.h"

void UProgressWaveState::Enter()
{
	Super::Enter();
	ChangeState(EWaveState::End);
	
}

void UProgressWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UProgressWaveState::Exit()
{
	Super::Exit();
}

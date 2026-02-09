// Fill out your copyright notice in the Description page of Project Settings.


#include "IntermissionWaveState.h"

void UIntermissionWaveState::Enter()
{
	Super::Enter();
	ChangeState(EWaveState::Begin);
}

void UIntermissionWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UIntermissionWaveState::Exit()
{
	Super::Exit();
}

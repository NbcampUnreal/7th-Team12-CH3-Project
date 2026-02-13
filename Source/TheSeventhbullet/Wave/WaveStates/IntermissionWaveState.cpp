// Fill out your copyright notice in the Description page of Project Settings.


#include "IntermissionWaveState.h"

void UIntermissionWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Intermission Wave"));
	
}

void UIntermissionWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//TODO : 다음 웨이브 까지 잠깐 쉬는시간으로, 시간 지나는 함수 등 필요
	ChangeState(EWaveState::Begin);
}

void UIntermissionWaveState::Exit()
{
	Super::Exit();
}

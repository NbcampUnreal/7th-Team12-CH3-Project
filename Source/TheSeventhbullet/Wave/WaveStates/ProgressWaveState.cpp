// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressWaveState.h"

void UProgressWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Progress Wave"));
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{}
	GM->StartWaveSpawn();
	
}

void UProgressWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//TODO : 몬스터가 다 죽었는지 체크하는 로직 필요
	//TODO : 다 죽었다면 State를 End로 변경
	ChangeState(EWaveState::End);
}

void UProgressWaveState::Exit()
{
	Super::Exit();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWaveState.h"

void UBaseWaveState::Initialize(UWaveStateMachine* InStateMachine)
{
	StateMachine = InStateMachine;
}

void UBaseWaveState::Enter()
{
	AMainGameState* GameState = GetGameState();
	if (GameState)
	{
		//GameState에서 해줘야할것들
	}
}

void UBaseWaveState::Tick(float DeltaTime)
{
}

void UBaseWaveState::Exit()
{
}

void UBaseWaveState::ChangeState(EWaveState NewState)
{
	if (StateMachine)
	{
		StateMachine->ChangeState(NewState);
	}
}

AMainGameMode* UBaseWaveState::GetGameMode() const
{
	if (StateMachine == nullptr)
	{
		return nullptr;
	}
	return StateMachine->GetOwner();
}

AMainGameState* UBaseWaveState::GetGameState() const
{
	AMainGameMode* GameMode = GetGameMode();
	if (GameMode == nullptr)
	{
		return nullptr;
	}
	return GameMode->GetGameState<AMainGameState>();
}

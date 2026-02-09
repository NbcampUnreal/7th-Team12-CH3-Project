// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveStateMachine.h"

#include "WaveStates/BaseWaveState.h"
#include "WaveStates/BeginWaveState.h"
#include "WaveStates/NoneWaveState.h"
#include "WaveStates/ProgressWaveState.h"
#include "WaveStates/IntermissionWaveState.h"

void UWaveStateMachine::Initalize(AMainGameMode* InOwner)
{
	Owner = InOwner;
	CurrentStateType = EWaveState::None;
	CurrentState = GetOrCreateState(EWaveState::None);
}

void UWaveStateMachine::ChangeState(EWaveState NewState)
{
	if (CurrentStateType == NewState)
	{
		return;
	}
	if (CurrentState)
	{
		CurrentState->Exit();
	}
	CurrentStateType = NewState;
	CurrentState = GetOrCreateState(NewState);
	
	if (CurrentState)
	{
		CurrentState->Enter();
	}
}

void UWaveStateMachine::Tick(float DeltaTime)
{
	if (CurrentState)
	{
		CurrentState->Tick(DeltaTime);
	}
}

UBaseWaveState* UWaveStateMachine::GetOrCreateState(EWaveState StateType)
{
	if (TObjectPtr<UBaseWaveState>* Found = StateCache.Find(StateType))
	{
		return *Found;
	}
	
	UClass* StateClass = nullptr;
	switch (StateType)
	{
	case EWaveState::None:
		StateClass = UNoneWaveState::StaticClass();
		break;
	case EWaveState::Begin:
		StateClass = UBeginWaveState::StaticClass();
		break;
	case EWaveState::Progress:
		StateClass = UProgressWaveState::StaticClass();
		break;
	case EWaveState::Intermission:
		StateClass = UIntermissionWaveState::StaticClass();
		break;
	}
	if (StateClass == nullptr)
	{
		return nullptr;
	}
	
	UBaseWaveState* NewState = NewObject<UBaseWaveState>(this, StaticClass());
	NewState->Initialize(this);
	StateCache.Add(StateType, NewState);
	
	return NewState;
}

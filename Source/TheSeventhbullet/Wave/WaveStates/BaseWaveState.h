// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaveState.h"
#include "TheSeventhbullet/System/MainGameMode.h"
#include "TheSeventhbullet/System/MainGameState.h"
#include "TheSeventhbullet/Wave/WaveStateMachine.h"
#include "UObject/Object.h"
#include "BaseWaveState.generated.h"

/**
 * Wave FSM : Begin -> Progress -> End -> Intermission -> Begin
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class THESEVENTHBULLET_API UBaseWaveState : public UObject
{
	GENERATED_BODY()
public:
	virtual void Initialize(UWaveStateMachine * InStateMachine);
public:
	UFUNCTION(BlueprintPure, Category = "WaveState")
	virtual EWaveState GetStateType() const PURE_VIRTUAL(UBaseWaveState::GetStateType, return EWaveState::None;);
public:
	virtual void Enter();
	virtual void Tick(float DeltaTime);
	virtual void Exit();
protected:
	void ChangeState(EWaveState NewState);
	AMainGameMode* GetGameMode() const;
	AMainGameState* GetGameState() const;
protected:
	UPROPERTY(Transient)
	TObjectPtr<UWaveStateMachine> StateMachine;
};

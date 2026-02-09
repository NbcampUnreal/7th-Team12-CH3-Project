// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWaveState.h"
#include "ProgressWaveState.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UProgressWaveState : public UBaseWaveState
{
	GENERATED_BODY()
public:
	virtual EWaveState GetStateType() const override {return EWaveState::Intermission;}
public:
	virtual void Enter() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Exit() override;
};

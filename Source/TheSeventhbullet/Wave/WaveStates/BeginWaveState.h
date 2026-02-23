// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWaveState.h"
#include "BeginWaveState.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UBeginWaveState : public UBaseWaveState
{
	GENERATED_BODY()
public:
	virtual EWaveState GetStateType() const override {return EWaveState::Begin;}
public:
	virtual void Enter() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Exit() override;
private:
	float DelayTimer =3.0f;
};

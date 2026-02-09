// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWaveState.h"
#include "NoneWaveState.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UNoneWaveState : public UBaseWaveState
{
	GENERATED_BODY()
public:
	virtual EWaveState GetStateType() const override {return EWaveState::None;}
};

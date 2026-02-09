// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WaveStates/WaveState.h"
#include "WaveStateMachine.generated.h"

class UBaseWaveState;
class AMainGameMode;
/**
 * 
 */
UCLASS(BlueprintType)
class THESEVENTHBULLET_API UWaveStateMachine : public UObject
{
	GENERATED_BODY()
public:
	void Initalize(AMainGameMode* InOwner);
	void ChangeState(EWaveState NewState);
	void Tick(float DeltaTime);
	
	FORCEINLINE EWaveState GetCurrentStateType() const {return CurrentStateType;}
	FORCEINLINE AMainGameMode* GetOwner() const {return Owner;}
	FORCEINLINE UBaseWaveState* GetCurrentState() const {return CurrentState;}

private:
	UBaseWaveState* GetOrCreateState(EWaveState StateType);
private:
	UPROPERTY()
	TObjectPtr<AMainGameMode> Owner;
	
	UPROPERTY()
	TObjectPtr<UBaseWaveState> CurrentState;

	EWaveState CurrentStateType = EWaveState::None;
	
	UPROPERTY()
	TMap<EWaveState, TObjectPtr<UBaseWaveState>> StateCache;
};

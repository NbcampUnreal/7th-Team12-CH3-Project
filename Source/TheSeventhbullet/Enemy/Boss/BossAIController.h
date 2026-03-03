// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAIControllerBase.h"
#include "BossAIController.generated.h"

class ABossCharacter;

UCLASS()
class THESEVENTHBULLET_API ABossAIController : public AEnemyAIControllerBase
{
	GENERATED_BODY()
	ABossAIController();
	
public:
	UFUNCTION()
	void BossCancelEvent();
	UFUNCTION()
	void BossPatternEndEvent();
	UFUNCTION()
	void BossHalfHealthEvent();
	UFUNCTION()
	void BossFinalPhaseEvent();
	
protected:
	FName bIsPatternEndKey=FName("bIsPatternEnd");
	FName bIsBossHalfHealthKey=FName("bIsHalfHealth");
	FName bIsBossFinalPhaseKey=FName("bIsFinalPhase");
	
	virtual void OnPossess(APawn* InPawn) override;
	


};

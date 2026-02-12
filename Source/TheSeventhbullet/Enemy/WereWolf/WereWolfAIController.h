// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WereWolfAIController.generated.h"

/**
 * 웨어울프의 BT를 적용하기 위한 AIConroller입니다.
 */
UCLASS()
class THESEVENTHBULLET_API AWereWolfAIController : public AAIController
{
	GENERATED_BODY()

protected:
	AWereWolfAIController();

	virtual void BeginPlay() override;

	
public:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* WereWolfBehaviorTree;
	virtual void OnPossess(APawn* InPawn) override;
};

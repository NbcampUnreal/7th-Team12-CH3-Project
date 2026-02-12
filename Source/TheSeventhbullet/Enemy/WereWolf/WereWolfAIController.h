// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WereWolfAIController.generated.h"

/**
 * 
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
	UFUNCTION()
	void UpdateBlackboardOnHit(bool bIsDetected);

private:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* WereWolfBehaviorTree;
	virtual void OnPossess(APawn* InPawn) override;
};

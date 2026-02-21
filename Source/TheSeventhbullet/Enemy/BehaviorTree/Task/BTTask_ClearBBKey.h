// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ClearBBKey.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UBTTask_ClearBBKey : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_ClearBBKey();
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree")
	FBlackboardKeySelector BBKey;	
	
private:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

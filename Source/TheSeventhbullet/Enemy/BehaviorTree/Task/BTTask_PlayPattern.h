// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayPattern.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UBTTask_PlayPattern : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_PlayPattern();
	
protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree")
	FBlackboardKeySelector IsPatternProgressKey;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString PatternName;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

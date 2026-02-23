// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DeadEnd.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UBTTask_DeadEnd : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_DeadEnd();
	
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FBlackboardKeySelector SelfActorKey;
	
	TObjectPtr<UBlackboardComponent> BBComp;
	
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

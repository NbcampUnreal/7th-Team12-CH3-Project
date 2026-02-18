// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MakeFalseBBKey.generated.h"

/**
 * Blackboard 키 값을 false로 만듭니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBTTask_MakeFalseBBKey : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTTask_MakeFalseBBKey();
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree")
	FBlackboardKeySelector BoolKey;	
	
private:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetBoolBBKey.generated.h"

/**
 * Blackboard 키 값을 해당 Bool 값으로 만듭니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBTTask_SetBoolBBKey : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTTask_SetBoolBBKey();
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree")
	FBlackboardKeySelector BoolKey;	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree")
	bool bMakeBoolTrue=false;
	
private:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

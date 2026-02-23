// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ClearBBKey.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBBKey::UBTTask_ClearBBKey()
{
	NodeName = TEXT("Make Key Clear");
}

EBTNodeResult::Type UBTTask_ClearBBKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp= OwnerComp.GetBlackboardComponent();;
	
	if (BBComp!=nullptr)
	{
		BBComp->ClearValue(BBKey.SelectedKeyName);
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetBoolBBKey.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetBoolBBKey::UBTTask_SetBoolBBKey()
{
	BoolKey.AddBoolFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_SetBoolBBKey,BoolKey));
}

EBTNodeResult::Type UBTTask_SetBoolBBKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	UBlackboardComponent* BBComp= OwnerComp.GetBlackboardComponent();;
	
	if (BBComp!=nullptr)
	{
		BBComp->SetValueAsBool(BoolKey.SelectedKeyName, bMakeBoolTrue);
		return EBTNodeResult::Succeeded;
	}
	
	
	return EBTNodeResult::Failed;
}


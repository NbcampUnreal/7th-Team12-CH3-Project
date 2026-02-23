// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MakeFalseBBKey.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MakeFalseBBKey::UBTTask_MakeFalseBBKey()
{
	NodeName = TEXT("Make Bool False");
	BoolKey.AddBoolFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_MakeFalseBBKey,BoolKey));
}

EBTNodeResult::Type UBTTask_MakeFalseBBKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	UBlackboardComponent* BBComp= OwnerComp.GetBlackboardComponent();;
	
	if (BBComp!=nullptr)
	{
		BBComp->SetValueAsBool(BoolKey.SelectedKeyName, false);
		return EBTNodeResult::Succeeded;
	}
	
	
	return EBTNodeResult::Failed;
}


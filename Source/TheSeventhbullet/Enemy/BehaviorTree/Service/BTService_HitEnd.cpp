// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_HitEnd.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_HitEnd::UBTService_HitEnd()
{
	NodeName = TEXT("Hit End");
	bNotifyCeaseRelevant = true;
	//탈출만 사용하기에 Tick=false
	bNotifyTick = false;
	//bIsHitKey는 Bool 만 올 수 있다.
	bIsHitKey.AddBoolFilter(this,GET_MEMBER_NAME_CHECKED(UBTService_HitEnd,bIsHitKey));
}

//서비스 노드에서 탈출하려고 할 때 실행합니다.
void UBTService_HitEnd::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	BBComp=OwnerComp.GetBlackboardComponent();
	if (BBComp)
	{
		BBComp->SetValueAsBool(bIsHitKey.SelectedKeyName, false);
	}
	
}

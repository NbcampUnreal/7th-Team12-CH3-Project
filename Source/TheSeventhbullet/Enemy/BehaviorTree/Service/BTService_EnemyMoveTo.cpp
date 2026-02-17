// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_EnemyMoveTo.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"

UBTService_EnemyMoveTo::UBTService_EnemyMoveTo()
{
	bNotifyCeaseRelevant = true;
	TargetActorKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(UBTService_EnemyMoveTo,TargetActorKey),AActor::StaticClass());
	AttackRadiusKey.AddFloatFilter(this,GET_MEMBER_NAME_CHECKED(UBTService_EnemyMoveTo,AttackRadiusKey));
}


void UBTService_EnemyMoveTo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	TObjectPtr<UBlackboardComponent> BBComp=OwnerComp.GetBlackboardComponent();
	if (BBComp == nullptr)
	{
		return;
	}
	AttackRadius=BBComp->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);
	TObjectPtr<AActor> TargetActor=Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (TargetActor == nullptr)
	{
		return;
	}
	
	OwnerComp.GetAIOwner()->MoveToActor(TargetActor, AttackRadius);
	
}

void UBTService_EnemyMoveTo::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	
	OwnerComp.GetAIOwner()->StopMovement();
}





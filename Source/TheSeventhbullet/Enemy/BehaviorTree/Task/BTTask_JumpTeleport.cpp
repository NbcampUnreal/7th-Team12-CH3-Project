// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_JumpTeleport.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_JumpTeleport::UBTTask_JumpTeleport()
{
	//TargetActor에는 AActor 클래스만 올 수 있다.
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_JumpTeleport, TargetActorKey), AActor::StaticClass());
	//SelfActor에는 AActor 클래스만 올 수 있다.
	SelfActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_JumpTeleport, SelfActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_JumpTeleport::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* TargetActor=Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
	AActor* SelfActor=Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(SelfActorKey.SelectedKeyName));
	if (TargetActor==nullptr||SelfActor==nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector TeleportLocation=TargetActor->GetActorLocation()+TargetActor->GetActorForwardVector()*TeleportDistance;
	
	//NavMesh를 가져옵니다.
	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	FNavLocation ProjectedLocation;
	//ProjectedLocation에 해당 TeleportLocation에서부터 500 반경의 가까운 갈수 있는 지점을 찾숩니다.
	bool bSuccess = NavSystem->ProjectPointToNavigation(TeleportLocation, ProjectedLocation, FVector(500.f, 500.f, 500.f));
	
	if (bSuccess)
	{
		SelfActor->SetActorLocation(ProjectedLocation.Location+FVector(0,0,TeleportHeight));
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
	
}

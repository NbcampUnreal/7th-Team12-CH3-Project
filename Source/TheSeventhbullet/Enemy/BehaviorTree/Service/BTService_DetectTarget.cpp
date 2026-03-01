// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_DetectTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_DetectTarget::UBTService_DetectTarget()
{
	NodeName = TEXT("Detect Target");
	Interval = 0.5f;
	RandomDeviation = 0.1f;

	//TargetActor에는 AActor 클래스만 올 수 있다.
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DetectTarget, TargetActorKey), AActor::StaticClass());
	//Distance에는 float만 올수 있다.
	DistanceKey.AddFloatFilter(this,GET_MEMBER_NAME_CHECKED(UBTService_DetectTarget, DistanceKey));
	//OnBecomeRelevant 활성화
	bNotifyBecomeRelevant = true;
}

void UBTService_DetectTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//(싱글플레이 전용)플레이어 폰을 Blackboard에 세팅합니다.
	APawn* PlayerPawn=UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerPawn);
	//SelfActor를 Blackboard에 세팅합니다.
	AAIController* AiController = OwnerComp.GetAIOwner();
	if (AiController == nullptr || AiController->GetPawn() == nullptr)
	{
		return;
	}
	APawn* EnemyPawn=OwnerComp.GetAIOwner()->GetPawn();
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(SelfActorKey.SelectedKeyName, EnemyPawn);
	
}

void UBTService_DetectTarget::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	//TargetActor, SelfActor를 Blackboard에서 가져옵니다.
	APawn* PlayerPawn=Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
	APawn* EnemyPawn=Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(SelfActorKey.SelectedKeyName));
	
	if (PlayerPawn==nullptr||EnemyPawn==nullptr)
	{
		return;
	}
	
	//AIController와 플레이어폰을 가져옵니다.
	AAIController* AiController = OwnerComp.GetAIOwner();
	if (AiController == nullptr || AiController->GetPawn() == nullptr)
	{
		return;
	}
	
	
	//거리를 구합니다.
	const float Distance = FVector::Dist(
		PlayerPawn->GetActorLocation(),
		EnemyPawn->GetActorLocation());
	
	//거리를 블랙보드 키에 반영합니다.
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceKey.SelectedKeyName,Distance);

}

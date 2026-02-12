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

	//DetectTarget에는 AActor 클래스만 올 수 있다.
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DetectTarget, TargetActorKey), AActor::StaticClass());
}

void UBTService_DetectTarget::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//AIController를 가져옵니다.
	AAIController* AiController = OwnerComp.GetAIOwner();
	if (AiController == nullptr || AiController->GetPawn() == nullptr)
	{
		return;
	}

	//플레이어 폰을 가져옵니다.
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(TargetActorKey.SelectedKeyName);
		UE_LOG(LogTemp,Warning,TEXT("No Pawn"));
		return;
	}

	//거리를 구합니다.
	const float Distance = FVector::Dist(
		AiController->GetPawn()->GetActorLocation(),
		PlayerPawn->GetActorLocation());
	
	UE_LOG(LogTemp,Warning,TEXT("%f"),Distance);
	
	//플레이어와 적 사이의 거리를 판단해 가까우면 BB에 설정하고, 멀면 버립니다.
	if (Distance <= DetectRadius)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerPawn);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(TargetActorKey.SelectedKeyName);
	}
}

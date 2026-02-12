#include "BTTask_FindRandomLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location");

	PatrolLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomLocation, PatrolLocationKey));
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AiController = OwnerComp.GetAIOwner();
	if (AiController == nullptr || AiController->GetPawn() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	//NavMesh를 가져옵니다.
	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	//몬스터의 현재 위치
	const FVector Origin = AiController->GetPawn()->GetActorLocation();
	FNavLocation RandomLocation;
	
	//몬스터의 위치에서 일정 반경의 무작워 지점
	const bool bFound = NavSystem->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomLocation);
	if (bFound)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

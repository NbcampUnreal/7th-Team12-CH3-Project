// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PlayPattern.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Boss/BossEnemyActorComponent.h"

UBTTask_PlayPattern::UBTTask_PlayPattern()
{
	IsPatternProgressKey.AddBoolFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_PlayPattern,IsPatternProgressKey));
}

EBTNodeResult::Type UBTTask_PlayPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp= OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	AActor* SelfActor=AIController->GetPawn();
	UBossEnemyActorComponent* BossEnemyActorComponent=SelfActor->FindComponentByClass<UBossEnemyActorComponent>();
	if (BBComp==nullptr||BossEnemyActorComponent==nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("BTTask Return"));
		return EBTNodeResult::Failed;
	}
	
	bool bIsPatternProgress=BBComp->GetValueAsBool(IsPatternProgressKey.SelectedKeyName);

	if (bIsPatternProgress==false)
	{
		UE_LOG(LogTemp,Warning,TEXT("PlayTaskPatter"));
		BossEnemyActorComponent->PlayPattern(PatternName);
	}
	
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_PlayPattern::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	return EBTNodeResult::Aborted;
}

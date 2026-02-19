// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DeadEnd.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UBTTask_DeadEnd::UBTTask_DeadEnd()
{
	NodeName = TEXT("Dead End");
	SelfActorKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_DeadEnd,SelfActorKey),AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_DeadEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BBComp = OwnerComp.GetBlackboardComponent();
	if (BBComp)
	{
		TWeakObjectPtr<ACharacter> SelfActor = Cast<ACharacter>(BBComp->GetValueAsObject(SelfActorKey.SelectedKeyName));
		if (SelfActor!=nullptr)
		{
			SelfActor->GetMesh()->bPauseAnims = true;
			SelfActor->GetMesh()->SetComponentTickEnabled(false);
			OwnerComp.StopTree(EBTStopMode::Safe);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}

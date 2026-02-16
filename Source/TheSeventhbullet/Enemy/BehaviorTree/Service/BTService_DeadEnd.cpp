// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_DeadEnd.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"


UBTService_DeadEnd::UBTService_DeadEnd()
{
	NodeName = TEXT("Dead End");
	bNotifyCeaseRelevant = true;
	//탈출만 사용하기에 Tick=false
	bNotifyTick = false;
	SelfActorKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(UBTService_DeadEnd,SelfActorKey),AActor::StaticClass());
}

void UBTService_DeadEnd::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	BBComp = OwnerComp.GetBlackboardComponent();
	if (BBComp)
	{
		SelfActor = Cast<ACharacter>(BBComp->GetValueAsObject(SelfActorKey.SelectedKeyName));
		if (SelfActor!=nullptr)
		{
			SelfActor->GetMesh()->bPauseAnims = true;
			SelfActor->GetMesh()->SetComponentTickEnabled(false);
			//StopTree를 해도 바로 Wait으로 이동하고, Tick도 꺼놓았기 때문에 크래시가 나지 않았다.
			OwnerComp.StopTree(EBTStopMode::Safe);
			SelfActor->UnPossessed();
		}
	}
}

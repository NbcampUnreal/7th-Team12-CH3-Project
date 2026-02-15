// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_DeadEnd.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UBTService_DeadEnd::UBTService_DeadEnd()
{
	NodeName = TEXT("Dead End");
	bNotifyCeaseRelevant = true;
	//탈출만 사용하기에 Tick=false
	bNotifyTick = false;
}

void UBTService_DeadEnd::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	BBComp=OwnerComp.GetBlackboardComponent();
	if (BBComp)
	{
		SelfActor=Cast<ACharacter>(BBComp->GetValueAsObject(TEXT("SelfActor")));
		SelfActor->GetMesh()->bPauseAnims = true;
		SelfActor->GetMesh()->SetComponentTickEnabled(false);
		SelfActor->UnPossessed();
	}
}

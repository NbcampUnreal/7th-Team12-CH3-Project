// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetFocus.h"

#include "AIController.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetFocus::UBTTask_SetFocus()
{
	SelfActorKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_SetFocus,SelfActorKey),AActor::StaticClass());
	TargetActorKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_SetFocus,TargetActorKey),AActor::StaticClass());
	SelfActorKey.AddFloatFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_SetFocus,SelfActorKey));
}

EBTNodeResult::Type UBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//EnemyCharacter, PlayerActor를 형변환하고 early return
	//블랙보드 안의 노드라 캐싱하지 않음
	UBlackboardComponent* BBComp=OwnerComp.GetBlackboardComponent();
	if (BBComp==nullptr)
	{
		return EBTNodeResult::Failed;
	}
	ACharacter* EnemyCharacter=Cast<ACharacter>(BBComp->GetValueAsObject(SelfActorKey.SelectedKeyName));
	if (EnemyCharacter==nullptr)
	{
		return EBTNodeResult::Failed;
	}
	AAIController* EnemyAIController=Cast<AAIController>(EnemyCharacter->GetController());
	
	//SetFocus 적용
	if (bIsSetFocus)
	{
		AActor* TargetActor=Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if (TargetActor==nullptr)
		{
			return EBTNodeResult::Failed;
		}
		
		//시선을 움직이는 방향으로 바라보지 않고, 컨트롤러 Yaw를 따르게 한다.
		EnemyCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		EnemyCharacter->bUseControllerRotationYaw = true;
		//TargetActor로 시선고정
		EnemyAIController->SetFocus(TargetActor);
	}
	//SetFocus 해제
	else
	{
		EnemyCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		EnemyCharacter->bUseControllerRotationYaw = false;
		//TargetActor로 시선해제
		EnemyAIController->ClearFocus(EAIFocusPriority::Gameplay);
	}	
	return EBTNodeResult::Succeeded;
}

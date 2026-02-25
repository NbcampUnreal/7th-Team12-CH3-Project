// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetCharacterSpeed.h"

#include "BehaviorTree/BlackBoardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetCharacterSpeed::UBTTask_SetCharacterSpeed()
{
	SelfActorKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_SetCharacterSpeed,SelfActorKey),AActor::StaticClass());
	EnemySpeed.AddFloatFilter(this,GET_MEMBER_NAME_CHECKED(UBTTask_SetCharacterSpeed,EnemySpeed));
}

EBTNodeResult::Type UBTTask_SetCharacterSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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
	float Speed=BBComp->GetValueAsFloat(EnemySpeed.SelectedKeyName);
	
	EnemyCharacter->GetCharacterMovement()->MaxWalkSpeed=Speed;
	
	return EBTNodeResult::Succeeded;
}

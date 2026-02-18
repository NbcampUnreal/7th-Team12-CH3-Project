// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_StopMovement.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTTask_StopMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Owner = OwnerComp.GetAIOwner();
	if (Owner == nullptr)
	{
		return EBTNodeResult::Aborted;
	}

	Owner->StopMovement();
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

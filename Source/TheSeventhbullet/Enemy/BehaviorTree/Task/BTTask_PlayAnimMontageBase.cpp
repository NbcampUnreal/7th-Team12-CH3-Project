// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PlayAnimMontageBase.h"
#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_PlayAnimMontageBase::UBTTask_PlayAnimMontageBase()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PlayAnimMontageBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Owner = OwnerComp.GetAIOwner();
	if (Owner == nullptr)
	{
		return EBTNodeResult::Aborted;
	}

	ACharacter* AiController = Cast<ACharacter>(Owner->GetPawn());
	if (AiController == nullptr || AnimMontage == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AiController->PlayAnimMontage(AnimMontage);
	return EBTNodeResult::InProgress;
}

void UBTTask_PlayAnimMontageBase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//AIController 가 없으면 Fail
	AAIController* Owner = OwnerComp.GetAIOwner();
	if (Owner == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	//캐릭터나 몽타주 둘중하나라도 없으면 Fail
	ACharacter* AICharacter = Cast<ACharacter>(Owner->GetPawn());
	if (AICharacter == nullptr || AnimMontage == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	//재생하고있는 애님인스턴스가 없으면 Succeeded
	UAnimInstance* AnimInstance = AICharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 재생중인 몽타주가 멈췄으면 Succeeded
	bool bStopped = AnimInstance->Montage_GetIsStopped(AnimMontage);
	if (bStopped)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_PlayAnimMontageBase::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_PlayAnimMontageBase::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                 EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

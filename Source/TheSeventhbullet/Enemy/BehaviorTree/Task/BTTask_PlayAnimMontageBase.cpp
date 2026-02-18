// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PlayAnimMontageBase.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "TheSeventhbullet/Enemy/EnemyBase.h"


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

	ACharacter* AiCharacter = Cast<ACharacter>(Owner->GetPawn());
	if (AiCharacter == nullptr || AnimMontageName == "None")
	{
		return EBTNodeResult::Failed;
	}
	AEnemyBase* AIEnemyCharacter=Cast<AEnemyBase>(AiCharacter);
	if(AIEnemyCharacter==nullptr)
	{
		return EBTNodeResult::Failed;
	}
	if (AIEnemyCharacter->ReturnthisMontage(AnimMontageName)==nullptr) return EBTNodeResult::Failed;
	AiCharacter->PlayAnimMontage(AIEnemyCharacter->ReturnthisMontage(AnimMontageName));
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
	if (AICharacter == nullptr || AnimMontageName == "")
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	AEnemyBase* AIEnemyCharacter=Cast<AEnemyBase>(AICharacter);
	if(AIEnemyCharacter==nullptr)
	{
		return;
	}

	//재생하고있는 애님인스턴스가 없으면 Succeeded
	UAnimInstance* AnimInstance = AICharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	//몽타주가 없으면 리턴
	if (AIEnemyCharacter->ReturnthisMontage(AnimMontageName)==nullptr)
	{
		return;
	}
	// 재생중인 몽타주가 멈췄으면 Succeeded
	bool bStopped = AnimInstance->Montage_GetIsStopped(AIEnemyCharacter->ReturnthisMontage(AnimMontageName));
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

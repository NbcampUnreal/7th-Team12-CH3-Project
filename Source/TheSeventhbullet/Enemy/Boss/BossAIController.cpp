// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAIController.h"

#include "BossEnemyActorComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


ABossAIController::ABossAIController()
{
}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UBossEnemyActorComponent* BossEnemyActorComponent=Enemy->FindComponentByClass<UBossEnemyActorComponent>();
	if (BossEnemyActorComponent!=nullptr)
	{
		BossEnemyActorComponent->OnBossPatternEnd.AddDynamic(this,&ABossAIController::BossPatternEndEvent);
		BossEnemyActorComponent->OnBossCanceled.AddDynamic(this,&ABossAIController::BossCancelEvent);
		BossEnemyActorComponent->OnBossFirstHalfHealth.AddDynamic(this,&ABossAIController::BossHalfHealthEvent);
		BossEnemyActorComponent->OnBossFinalPhase.AddDynamic(this,&ABossAIController::BossFinalPhaseEvent);
	}
}

void ABossAIController::BossCancelEvent()
{
	if (BBComp==nullptr) return;
	BBComp->SetValueAsBool(FName("bIsCanceled"), true);
}

void ABossAIController::BossPatternEndEvent()
{
	if (BBComp==nullptr) return;
	BBComp->SetValueAsBool(bIsPatternEndKey, true);
}

void ABossAIController::BossHalfHealthEvent()
{
	if (BBComp==nullptr) return;
	BBComp->SetValueAsBool(bIsBossHalfHealthKey, true);
}

void ABossAIController::BossFinalPhaseEvent()
{
	if (BBComp==nullptr) return;
	BBComp->SetValueAsBool(bIsBossFinalPhaseKey, true);
}

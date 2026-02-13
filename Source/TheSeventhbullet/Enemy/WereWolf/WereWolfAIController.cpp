// Fill out your copyright notice in the Description page of Project Settings.


#include "WereWolfAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WereWolfCharacter.h"

AWereWolfAIController::AWereWolfAIController()
{
}

void AWereWolfAIController::BeginPlay()
{
    Super::BeginPlay();
    
}

void AWereWolfAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AWereWolfAIController::OnPossess(APawn* InPawn)
{
    //빙의 시 BehaviorTree를 시작합니다.
    Super::OnPossess(InPawn);
}

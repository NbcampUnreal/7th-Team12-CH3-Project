// Fill out your copyright notice in the Description page of Project Settings.


#include "WereWolfAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WereWolfCharacter.h"

AWereWolfAIController::AWereWolfAIController()
{
    WereWolfBehaviorTree = nullptr;
}

void AWereWolfAIController::BeginPlay()
{
    Super::BeginPlay();
    
}

void AWereWolfAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AWereWolfAIController::UpdateBlackboardOnHit(bool bIsDetected)
{
    GetBlackboardComponent()->SetValueAsBool(TEXT("isSearchArea"),bIsDetected);
}
void AWereWolfAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (WereWolfBehaviorTree)
    {
        bool bSuccess = RunBehaviorTree(WereWolfBehaviorTree);
        UE_LOG(LogTemp,Warning,TEXT("%s"),bSuccess?TEXT("TRUE"):TEXT("FALSE"));
    }
}

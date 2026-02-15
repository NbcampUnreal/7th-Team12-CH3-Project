// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DeadEnd.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UBTService_DeadEnd : public UBTService
{
	GENERATED_BODY()
	UBTService_DeadEnd();
	
protected:
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	TWeakObjectPtr<ACharacter> SelfActor;
	TObjectPtr<UBlackboardComponent> BBComp;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_HitEnd.generated.h"

/**
 * 단순히 Hit되었을시 진입하여 Hit을 false로 바꾸어 준다.
 */
UCLASS()
class THESEVENTHBULLET_API UBTService_HitEnd : public UBTService
{
	GENERATED_BODY()
	UBTService_HitEnd();
	
protected:
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

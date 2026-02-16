// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_HitEnd.generated.h"

/**
 * Hit의 중복 실행을 방지합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBTService_HitEnd : public UBTService
{
	GENERATED_BODY()
	UBTService_HitEnd();
	
public:
	//bool BB키
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FBlackboardKeySelector bIsHitKey;
	
	
protected:
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:
	TObjectPtr<UBlackboardComponent> BBComp;
};

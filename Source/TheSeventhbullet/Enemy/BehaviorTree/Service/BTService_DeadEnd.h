// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DeadEnd.generated.h"

/**
 * 캐릭터 사망 이후 애니메이션 일시정지, BT 정지 관련 처리를 합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBTService_DeadEnd : public UBTService
{
	GENERATED_BODY()
	UBTService_DeadEnd();
	
public:
	//bool BB 키
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FBlackboardKeySelector SelfActorKey;
protected:
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	TObjectPtr<UBlackboardComponent> BBComp;

};

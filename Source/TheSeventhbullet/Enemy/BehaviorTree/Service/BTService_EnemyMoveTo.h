// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_EnemyMoveTo.generated.h"

/**
 * Move To의 AttackRadius를 BB값으로 바꾸기 위해 서비스를 실행합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBTService_EnemyMoveTo : public UBTService
{
	GENERATED_BODY()
	UBTService_EnemyMoveTo();
protected:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FBlackboardKeySelector AttackRadiusKey;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree")
	float AttackRadius;
	
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};

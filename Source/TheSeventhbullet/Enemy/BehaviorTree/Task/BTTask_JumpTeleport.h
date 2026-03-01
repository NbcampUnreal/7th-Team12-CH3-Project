// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_JumpTeleport.generated.h"

/**
 * TargetActor 전방 TeleportDistance만큼 떨어진 곳이 갈수 있다면, 텔레포트합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBTTask_JumpTeleport : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_JumpTeleport();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SelfActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float TeleportDistance=300.0f;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float TeleportHeight=100.0f;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};

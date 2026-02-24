// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetFocus.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UBTTask_SetFocus : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_SetFocus();
	
protected:
	//블랙보드 키 셀렉터
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FBlackboardKeySelector SelfActorKey;
	UPROPERTY(EditAnyWhere,BlueprintReadOnly,Category="SetFocus")
	bool bIsSetFocus=false;
	
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};

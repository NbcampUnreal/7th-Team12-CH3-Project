// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * BlendSpace Get, Set 이 포함된 AnimInstance입니다.
 */
UCLASS()
class THESEVENTHBULLET_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Enemy|BlendSpace")
	void SetEnemyBlendSpace(UBlendSpace* BlendSpace);
	UFUNCTION(BlueprintCallable, Category="Enemy|BlendSpace")
	UBlendSpace* GetEnemyBlendSpace();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|BlendSpace", meta=(AllowPrivateAccess="true"))
	UBlendSpace* EnemyBlendSpace;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|BlendSpace", meta=(AllowPrivateAccess="true"))
	float Speed=0;
	
	void NativeUpdateAnimation(float DeltaSeconds);
};

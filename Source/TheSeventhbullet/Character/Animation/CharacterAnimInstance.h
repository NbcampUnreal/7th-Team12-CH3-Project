// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float MovementSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Direction;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	bool bIsDodging;
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	bool bIsAiming;
	
	UFUNCTION()
	void AnimNotify_Throw();
private:
	UPROPERTY()
	TObjectPtr<class ACharacter> OwnerCharacter;
};

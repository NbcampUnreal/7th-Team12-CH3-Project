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
	UPROPERTY(blueprintreadwrite, Category = "Movement")
	float MovementSpeed;

	UPROPERTY(blueprintreadwrite, Category = "Movement")
	bool bIsFalling;
	UPROPERTY(blueprintreadwrite, Category = "Action")
	bool bIsDodging;
	UPROPERTY(blueprintreadwrite, Category = "Action")
	bool bIsAiming;
	
private:
	UPROPERTY()
	TObjectPtr<class ACharacter> OwnerCharacter;
};

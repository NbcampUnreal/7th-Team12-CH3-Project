// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

struct FInputActionValue;

UCLASS()
class THESEVENTHBULLET_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:
	virtual void BeginPlay() override;
public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	int32 MaxHP;	// 최대 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	int32 CurrentHP;	// 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	int32 MaxStamina;	// 최대 스테미나
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	int32 CurrentStamina;	// 현재 스테미나
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	float MaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	float SprintMultifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	UCameraComponent* Camera;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void PlayerMove(const FInputActionValue& value);
	void PlayerLook(const FInputActionValue& value);
	void PlayerStartSprint(const FInputActionValue& value);
	void PlayerStopSprint(const FInputActionValue& value);
	void PlayerDodge(const FInputActionValue& value);
	void PlayerAim(const FInputActionValue& value);
	void PlayerFire(const FInputActionValue& value);
	void PlayerInteract(const FInputActionValue& value);
	void PlayerOpenInventory(const FInputActionValue& value);
};

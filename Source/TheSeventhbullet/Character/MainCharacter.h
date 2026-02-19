// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

class UInventoryComponent;
class UCombatComponent; // CombatComponent 전방선언
class AWeaponBase; // WeaponBase 전방선언
class UInputAction;
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
	/*virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
		) override;*/
public:	
	
#pragma region Status
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	int32 MaxHP;	// 최대 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	int32 CurrentHP;	// 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	int32 MaxStamina;	// 최대 스테미나
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	int32 CurrentStamina;	// 현재 스테미나
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	int32 DodgeCost;	// 회피 소모 스테미나
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	float DodgeDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	float MaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	float SprintMultifier;
	
#pragma endregion
	
#pragma region State
	
	bool bIsDodge = false;		// 회피 동작 상태
	bool bIsInvicible = false;	// 무적 상태
	bool bIsAiming = false;		// 조준 상태
#pragma endregion
	
#pragma region Camera
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	UCameraComponent* Camera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	FVector NormalSpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	FVector AimingSpringArm;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float NormalArmLength; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float AimingArmLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float CameraInterpSpeed;
#pragma endregion

#pragma region Actions
	void PlayerMove(const FInputActionValue& value);
	void PlayerLook(const FInputActionValue& value);
	void PlayerStartSprint(const FInputActionValue& value);
	void PlayerStopSprint(const FInputActionValue& value);
	void PlayerDodge(const FInputActionValue& value);
	void PlayerDodgeFinished(const FInputActionValue& value);
	void PlayerAim(const FInputActionValue& value);
	void PlayerAimFinished(const FInputActionValue& value);
	void PlayerFire(const FInputActionValue& value);
	void FinishFire(const FInputActionValue& value);
	void PlayerSkill(const FInputActionValue& value);
	void PlayerInteract(const FInputActionValue& value);
	void PlayerOpenInventory(const FInputActionValue& value);
	void PlayerReload(const FInputActionValue& value);
#pragma endregion

#pragma region Skill
	
	UPROPERTY(EditAnywhere, Category="Skill")
	TSubclassOf<class APlayerSkill> PlayerSkillClass;
	UPROPERTY(EditAnywhere, Category="Skill")
	FVector MuzzleOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	FName HandSocketName;
	
#pragma endregion
	
#pragma region Utilities
	
	bool IsDodge();
	bool IsInvicible();
	
#pragma endregion
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Tick(float DeltaTime) override;
	
	// 주현 : 테스트용 무기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<AWeaponBase> CurrentWeapon;
	
	// 주현 : CombatComponent
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="CombatComponent")
	TObjectPtr<UCombatComponent> CombatComponent;
	
	// Inventory
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

class UInventoryComponent;
class UCombatComponent; // 주현 : CombatComponent 전방선언
class UEquipmentComponent; // 주현 : EquipmentComponent 전방선언
class UWeaponDataAsset; // 주현 : WeaponDataAsset 전방선언
class UCombatComponent; // CombatComponent 전방선언
class UEquipmentComponent; // EquipmentComponent 전방선언
class UStatusComponent; // StatusComponent 전방선언
class AWeaponBase; // WeaponBase 전방선언
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class IInteractableInterface;

struct FInputActionValue;

UENUM(BlueprintType)
enum class EAnimState : uint8
{
	None,
	Skill,				
	Aim_Rifle,
	Aim_Shotun,
	Aim_Pistol,
	Fire_Rifle,  
	Fire_ShotGun,
	Fire_Pistol,
	Fire_Aim_Rifle,  
	Fire_Aim_ShotGun,
	Fire_Aim_Pistol, 
	Reload_Rifle,  
	Reload_ShotGun,
	Reload_Pistol, 
	Dodge
};


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
	
	EAnimState CurrentState = EAnimState::None;	// 애니메이션 상태별 출력을 위한 Enum
	
	UPROPERTY(EditDefaultsOnly, Category="Animation|Montage")
	TMap<EAnimState, TObjectPtr<UAnimMontage>> MontagesMap;	// 애니메이션 몽타주 저장 변수
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	FCharacterStat TotalStatus;
	
	UPROPERTY()
	int32 Gold = 0;
	
public:	
	
#pragma region Status
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	float SprintMultiplier;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	float AimMultiplier;
	
#pragma endregion

#pragma region Components
	
	// 주현 : WeaponMeshComponent 장착하면 보이는 무기 메쉬
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Component")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;
	// 주현 : CombatComponent
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Component")
	TObjectPtr<UCombatComponent> CombatComponent;
	// 주현 : EquipmentComponent
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Component")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;
	// 주현 : StatusComponent
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Component")
	TObjectPtr<UStatusComponent> StatusComponent;
	
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
	void ToggleEscMenu(const FInputActionValue& value);

#pragma endregion

#pragma region Skill
	
	UPROPERTY(EditAnywhere, Category="Skill")
	TSubclassOf<class APlayerSkill> PlayerSkillClass;
	UPROPERTY(EditAnywhere, Category="Skill")
	FVector MuzzleOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	FName HandSocketName;
	
	void ThrowGrenade();	// 스킬 실행
	
#pragma endregion
	
#pragma region Utilities
	
	bool IsDodge();
	bool IsInvicible();
	bool IsAiming();
	
#pragma endregion
	
	void PlayAnimMotageByState(EAnimState AnimState);
	void EndedAnimMontage(UAnimMontage* Montage, bool Interrupted);
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Tick(float DeltaTime) override;
	
	// 주현 : 테스트용 무기 슬롯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TestWeapon")
	TObjectPtr<UWeaponDataAsset> TestWeapon;
	
	// 주현 : SoulGem 장착할 때마다 SoulGem의 스탯들을 모아서 StatusComponent에 재적용.
	UFUNCTION(BlueprintCallable)
	void HandleEquipmentChanged();
		
#pragma region Interaction
	void SetCurrentInteractable(IInteractableInterface* Interactable);
	IInteractableInterface* GetCurrentInteractable() const;
private:
	IInteractableInterface* CurrentInteractable = nullptr;
public:
#pragma endregion

	// Inventory
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	//현석 : AI 퍼셉션 감지 대상용 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* StimuliSource;
	
	UFUNCTION(BlueprintPure, Category = "Status")
	const FCharacterStat& GetTotalStatus() const;
	
	void SetTotalStatus(const FCharacterStat& NewStatus);
	
	UFUNCTION()
	void OnDeath();
public:
	UFUNCTION()
	int32 GetGold();
	UFUNCTION()
	void AddGold(int32 Amount);
};

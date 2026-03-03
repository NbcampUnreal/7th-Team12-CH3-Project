// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"


class UInventoryComponent;
class UCombatComponent; // 주현 : CombatComponent 전방선언
class UWeaponDataAsset; // 주현 : WeaponDataAsset 전방선언
class UEquipmentComponent; // EquipmentComponent 전방선언
class UStatusComponent; // StatusComponent 전방선언
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class IInteractableInterface;

struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);

UENUM(BlueprintType)
enum class EAnimState : uint8
{
	None,
	Skill,				
	Dodge,
	Death
};


UCLASS()
class THESEVENTHBULLET_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category="Animation|Montage")
	TMap<EAnimState, TObjectPtr<UAnimMontage>> MontagesMap;	// 애니메이션 몽타주 저장 변수
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	FCharacterStat TotalStatus;
	
	UPROPERTY()
	int32 Gold = 10000;
	
public:	
	EAnimState CurrentState = EAnimState::None;	// 애니메이션 상태별 출력을 위한 Enum
	
#pragma region Status
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	float SprintMultiplier;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	float AimMultiplier;

	// 스태미나: Dodge 소모량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stamina")
	float DodgeStaminaCost = 20.f;

	// 스태미나: 초당 회복량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stamina")
	float StaminaRegenRate = 10.f;

	// 스태미나: 소모 후 회복 시작까지 대기 시간(초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stamina")
	float StaminaRegenDelay = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Potion")
	float HealAmount = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Potion")
	int32 MaxPotion = 7;
	
	int32 CurrentPotion =MaxPotion;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	void HealHP();
	void ResetPotion();

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
	bool bIsReload = false;	// 장전 상태
	bool bIsFireButtonPressed = false;	// 사격 인풋 
	bool bIsFire = false;	// 사격 상태
	
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
	float NormalFOV;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float AimingFOV;

	float SmoothedCameraZ;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float CameraLerpValueZ = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float CameraInterpSpeed;

#pragma endregion

#pragma region Actions
	
	void PlayerMove(const FInputActionValue& value);
	void PlayerLook(const FInputActionValue& value);
	void PlayerStartSprint(const FInputActionValue& value);
	void PlayerStopSprint(const FInputActionValue& value);
	void PlayerDodge(const FInputActionValue& value);
	void PlayerAim(const FInputActionValue& value);
	void PlayerAimFinished(const FInputActionValue& value);
	void PlayerFire(const FInputActionValue& value);
	void FinishFire(const FInputActionValue& value);
	void PlayerSkill(const FInputActionValue& value);
	void FinishSkill(const FInputActionValue& value);
	void PlayerInteract(const FInputActionValue& value);
	void PlayerOpenInventory(const FInputActionValue& value);
	void PlayerStartReload(const FInputActionValue& value);
	void PlayerFinishReload(const FInputActionValue& value);
	void PlayerPotion(const FInputActionValue& value);
	void ToggleEscMenu(const FInputActionValue& value);

#pragma endregion

#pragma region Skill
	
	UPROPERTY(EditAnywhere, Category="Skill")
	TSubclassOf<class APlayerSkill> PlayerSkillClass;
	UPROPERTY(EditAnywhere, Category="Skill")
	FVector MuzzleOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	FName HandSocketName;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
	float SkillCoolTime = 5.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Skill")
	float RemainSkillCoolTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Skill")
	bool bCanUseSkill = true;
	
	FTimerHandle SkillCoolTimerHandle;
	
	void ThrowGrenade();	// 스킬 실행
	void ResetSkillCoolTime();
	void ShowWeaponMesh();
	float GetSkillCoolTime();
	
	bool bIsUsingSkill = false;
	
#pragma endregion
	
#pragma region Combat
	
	FTimerHandle FireTimerHandle;	// 사격 간격 TimeHandle
	FTimerHandle ReloadTimerHandle;	// 장전 TimeHandle
	FTimerHandle AimFireStateTimerHandle;	// 조준사격 카메라 분리를 위한 TimeHandle
	
	void Fire();
	void Reload();
	
	void UpdateRotationState();	// 조준시 캐릭터 Rotation State 업데이트 
	void ResetFireState();
	
#pragma endregion
	
#pragma region Utilities
	
	bool IsDodge();
	bool IsInvicible();
	bool IsAiming();
	bool IsFiring();
	bool IsFalling();
	bool IsUseSkill();
	
#pragma endregion
	
	void PlayAnimMotageByState(EAnimState AnimState);
	void EndedAnimMontage(UAnimMontage* Montage, bool Interrupted);
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	
	// 주현 : 테스트용 무기 슬롯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TestWeapon")
	TObjectPtr<UWeaponDataAsset> TestWeapon;
	
	// 주현 : SoulGem 장착할 때마다 SoulGem의 스탯들을 모아서 StatusComponent에 재적용.
	UFUNCTION(BlueprintCallable)
	void HandleEquipmentChanged();
		
protected:
	void PauseAnim();
	
#pragma region Interaction
public:
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

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FOnHPChanged OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FOnStaminaChanged OnStaminaChanged;

	float GetCurrentHP() const { return CurrentHP; }
	float GetMaxHP() const { return static_cast<float>(TotalStatus.HP); }
	float GetCurrentStamina() const { return CurrentStamina; }
	float GetMaxStamina() const { return static_cast<float>(TotalStatus.Stamina); }
	
	void SetCurrentHP(const float NewHP) { CurrentHP = NewHP; };

	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void Revive();
	
	UFUNCTION()
	void LoadData(FCharacterStat& LoadTotalCharacterStatus, int32 CharacterGold);

	UFUNCTION()
	int32 GetGold();
	UFUNCTION()
	void AddGold(int32 Amount);

private:
	float CurrentHP = 0.f;
	float CurrentStamina = 0.f;

	// 스태미나 회복 딜레이 타이머
	FTimerHandle StaminaRegenTimerHandle;
	bool bCanRegenStamina = true;

	void StartStaminaRegenCooldown();
	void OnStaminaRegenReady();
};

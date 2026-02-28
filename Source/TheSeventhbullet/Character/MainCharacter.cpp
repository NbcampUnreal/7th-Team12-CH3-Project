#include "MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainPlayerController.h"
#include "PlayerSkill.h"
#include "TheSeventhbullet/Interaction/InteractableInterface.h"
#include "Animation/CharacterAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Component/CombatComponent.h" // 주현 : CombatComponent
#include "Component/EquipmentComponent.h" // 주현 : EquipmentComponent
#include "Component/StatusComponent.h" // StatusComponent
#include "DataAsset/WeaponDataAsset.h"
#include "Inventory/InventoryComponent.h" // Inventory
#include "UI/UITags.h"
#include "Manager/UIManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"


AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TotalStatus.Speed = 600.0f;
	TotalStatus.HP = 100;
	TotalStatus.Attack = 100;
	TotalStatus.Defence = 10;
	TotalStatus.CriticalChance = 0.15f;
	TotalStatus.CriticalDamage = 1.5f;
	
	SprintMultiplier = 1.5f;
	AimMultiplier = 0.8f;
	NormalArmLength = 400.0f; 
	AimingArmLength = 200.0f;
	NormalSpringArm = FVector(0.0f, 30.0f, 60.0f);
	AimingSpringArm = FVector(0.0f, 90.0f, 80.0f);
	NormalFOV = 90.0f;
	AimingFOV = 70.0f;
	CameraInterpSpeed = 15.0f;
	MuzzleOffset = FVector(300.0f, 0.0f, 0.0f);
	HandSocketName = FName("Muzzle_01");
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation= true;
	SpringArm->TargetArmLength = NormalArmLength;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1000.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = TotalStatus.Speed;
	
	bIsDodge = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	
	// 주현 : WeaponMeshComponent 초기화
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComp"));
	WeaponMeshComponent->SetupAttachment(GetMesh(), TEXT("weapon_r"));
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 무기가 캐릭터나 카메라랑 충돌나서 끔
	// 주현 : CombatComponent 초기화
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));
	// 주현 : EquipmentComponent 초기화
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));
	// 주현 : StatusComponent 초기화
	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("Status"));
	
  InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));

	//현석 : AI 퍼셉션 감지 대상 컴포넌트 추가, 태그 추가
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	Tags.Add(FName("Player"));
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[MainCharacter] BeginPlay - World: %s, Name: %s"), GetWorld() ? *GetWorld()->GetName() : TEXT("NULL"), *GetName());

	if (AMainPlayerController* PC = Cast<AMainPlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PC->InputMappingContext, 0);
		}
	}
	
	// 주현 : 테스트용 무기 장착
	EquipmentComponent->EquipWeaponData(TestWeapon);
	
	// 주현 : EquipmentComponent의 OnEquipmentChanged.Broadcast()를 호출할 때, HandleEquipmentChanged()를 실행시키기 위한 코드
	if (EquipmentComponent && StatusComponent)
	{
		EquipmentComponent->OnGemEquipmentChanged.AddDynamic(this, &AMainCharacter::HandleEquipmentChanged);
		HandleEquipmentChanged();
	}

	// TODO: 테스트용 - 나중에 제거
	if (InventoryComponent)
	{
		FPrimaryAssetId PotionID(FPrimaryAssetType("Item"), FName("DA_HealthPotion"));
		InventoryComponent->AddItem(PotionID, 3);
	}
}

void AMainCharacter::ThrowGrenade()
{
	// 클래스 할당 확인
	if (PlayerSkillClass)
	{
		UWorld* World = GetWorld();
		
		if (World)
		{
			
			
			// FRotator SpawnRotation = GetControlRotation();	// 카메라 회전
			// FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);	// 캐릭터 위치 + 앞쪽 오프셋
			
			FVector SpawnLocation;
			FRotator SpawnRotation;
			
			// 소켓 존재 여부 확인
			if (GetMesh()->DoesSocketExist(HandSocketName))
			{
				// 소켓 월드 위치 가져오기
				SpawnLocation = GetMesh()->GetSocketLocation(HandSocketName);
			}
			else
			{
				SpawnRotation = GetControlRotation();	// 카메라 회전
				SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);	// 캐릭터 위치 + 앞쪽 오프셋
			}
			
			// 카메라의 위치, 회전값
			FVector CameraLocation;
			FRotator CameraRotation;
			Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);
			
			// 카메라 방향 끝점 계산
			float TraceDistance = 5000.0f;
			FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * TraceDistance);
			
			// 레이트레이싱 변수
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);	// 본인 충돌 제외
			
			FVector TargetLocation = TraceEnd;
			
			if (World->LineTraceSingleByChannel(HitResult, CameraLocation, TraceEnd,ECC_Visibility, QueryParams))
			{
				TargetLocation = HitResult.ImpactPoint;	// 벽, 타겟 위치를 그 맞은 지점으로 갱신
			}
			
			FVector DirectionToTarget = TargetLocation - SpawnLocation;
			DirectionToTarget = DirectionToTarget.GetSafeNormal();
			
			SpawnRotation = DirectionToTarget.Rotation();
			
			float SpawnOffsetDistance = 40.0f;
			SpawnLocation = SpawnLocation + (DirectionToTarget * SpawnOffsetDistance);
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;	// 누가 던졌는지 기록 추후 데미지 판정에 씀
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
			World->SpawnActor<APlayerSkill>(PlayerSkillClass, SpawnLocation, SpawnRotation, SpawnParams);
		}
	}
}

void AMainCharacter::Fire()
{
	bIsFire = true;
	
	if (CombatComponent == nullptr || EquipmentComponent->CurrentWeapon == nullptr)	return;
	
	UpdateRotationState();
	
	float ResetInterval = EquipmentComponent->CurrentWeapon->FireInterval + 0.3;
	
	GetWorld()->GetTimerManager().SetTimer(
		AimFireStateTimerHandle, 
		this, 
		&AMainCharacter::ResetFireState, 
		ResetInterval, 
		false 
	);
	
	int CurrentAmmo = CombatComponent->GetCurrentAmmo();
	if (CurrentAmmo <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		Reload();
		return;
	}
	
	UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	// 남태 : TestWeapon 나중 데이터에셋 변수 바뀌면 수정 해야함
	TSoftObjectPtr<UAnimMontage> SelectedSoftMontage;
	SelectedSoftMontage = EquipmentComponent->CurrentWeapon->AttackMontage;
	
	UAnimMontage* MontageToPlay = SelectedSoftMontage.Get();
	
	if(!MontageToPlay)
	{
		MontageToPlay = EquipmentComponent->CurrentWeapon->AttackMontage.LoadSynchronous();
	}
	
	if (AnimInstance)
	{
		if (MontageToPlay)
		{
			float Duration = AnimInstance->Montage_Play(MontageToPlay);
		
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AMainCharacter::EndedAnimMontage);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, MontageToPlay);
		}
	}
	// FRotator CharacterRotation = GetBaseAimRotation();
	// CharacterRotation.Pitch = 0.0f;
	// CharacterRotation.Roll = 0.0f;
	// SetActorRotation(CharacterRotation);
	
	CombatComponent->StartFire(); // 발사 시작
	
	//현석 : 청각 이벤트 발생
	UAISense_Hearing::ReportNoiseEvent(
		   GetWorld(),
		   GetActorLocation(),  // 클릭한 위치
		   1.0f,               // Loudness
		  this,          // Instigator
		   2000.0f             // MaxRange
	   );
	UE_LOG(LogTemp,Warning,TEXT("Hearing Event Accured!"));
}

void AMainCharacter::Reload()
{
	if (CombatComponent == nullptr || EquipmentComponent->CurrentWeapon == nullptr)	return;
		
	bIsReload = true;
	
	UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
		
	UAnimMontage* MontageToPlay = EquipmentComponent->CurrentWeapon->ReloadMontage.Get();
	if(!MontageToPlay)	MontageToPlay = EquipmentComponent->CurrentWeapon->ReloadMontage.LoadSynchronous();
	
	if (AnimInstance && MontageToPlay)
	{
		float Duration = AnimInstance->Montage_Play(MontageToPlay);
	
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AMainCharacter::EndedAnimMontage);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, MontageToPlay);
	}
	
	//CombatComponent->Reload();	
}

bool AMainCharacter::IsDodge()
{
	return bIsDodge;
}

bool AMainCharacter::IsInvicible()
{
	return bIsInvicible;
}

bool AMainCharacter::IsAiming()
{
	return bIsAiming;
}

bool AMainCharacter::IsFiring()
{
	return bIsFire;
}

bool AMainCharacter::IsFalling()
{
	return GetCharacterMovement()->IsFalling();
}

void AMainCharacter::PlayAnimMotageByState(EAnimState AnimState)
{
	if (TObjectPtr<UAnimMontage>* FoundMontage = MontagesMap.Find(AnimState))
	{
		UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
		
		if (AnimInstance && *FoundMontage)
		{
			AnimInstance->Montage_Play(*FoundMontage);
			
			CurrentState = AnimState;
			
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AMainCharacter::EndedAnimMontage);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, *FoundMontage);
		}
	}
}

void AMainCharacter::EndedAnimMontage(UAnimMontage* Montage, bool Interrupted)
{
	CurrentState = EAnimState::None;
	
	bIsDodge = false;
	bIsInvicible = false;
	
	UpdateRotationState();
	
	// if (bIsAiming)
	// {
	// 	// 조준시 카메라 정면을 바라보게 고정
	// 	bUseControllerRotationYaw = true;
	// 	GetCharacterMovement()->bOrientRotationToMovement = false;
	// }
	// else
	// {
	// 	// 비조준시 카메라 고정 해제
	// 	bUseControllerRotationYaw = false;
	// 	GetCharacterMovement()->bOrientRotationToMovement = true;
	// }
	
	UAnimMontage* ReloadMontage = EquipmentComponent->CurrentWeapon->ReloadMontage.Get();
	if (Montage == ReloadMontage)
	{
		bIsReload = false;
		
		if (bIsFireButtonPressed)
		{			
			float FiraRate = EquipmentComponent->CurrentWeapon->FireInterval;
			if (FiraRate > 0.0f)
			{
				Fire();
				GetWorldTimerManager().SetTimer(
					FireTimerHandle,
					this,
					&AMainCharacter::Fire,
					FiraRate,
					true
				);
			}
		}
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* InputComponents = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMainPlayerController* PC = Cast<AMainPlayerController>(GetController()))
		{
			// Move 바인딩
			InputComponents->BindAction(
				PC->MoveAction,
				ETriggerEvent::Triggered,
				this,
				&AMainCharacter::PlayerMove
			);
			
			// Look 바인딩
			InputComponents->BindAction(
				PC->LookAction,
				ETriggerEvent::Triggered,
				this,
				&AMainCharacter::PlayerLook
			);
			
			// StartSprint 바인딩
			InputComponents->BindAction(
				PC->SprintAction,
				ETriggerEvent::Triggered,
				this,
				&AMainCharacter::PlayerStartSprint
			);
			
			// StopSprint 바인딩
			InputComponents->BindAction(
				PC->SprintAction,
				ETriggerEvent::Completed,
				this,
				&AMainCharacter::PlayerStopSprint
			);
			
			// Dodge 바인딩
			InputComponents->BindAction(
				PC->DodgeAction,
				ETriggerEvent::Started,
				this,
				&AMainCharacter::PlayerDodge
			);
			
			// Aim 바인딩
			InputComponents->BindAction(
				PC->AimAction,
				ETriggerEvent::Started,
				this,
				&AMainCharacter::PlayerAim
			);
			
			// Release Aim 바인딩
			InputComponents->BindAction(
				PC->AimAction,
				ETriggerEvent::Completed,
				this,
				&AMainCharacter::PlayerAimFinished
			);
			
			// Fire 바인딩
			InputComponents->BindAction(
				PC->FireAction,
				ETriggerEvent::Started,
				this,
				&AMainCharacter::PlayerFire
			);
			
			// Finish Fire 바인딩
			InputComponents->BindAction(
				PC->FireAction,
				ETriggerEvent::Completed,
				this,
				&AMainCharacter::FinishFire
			);
			
			// SKill 바인딩
			InputComponents->BindAction(
				PC->SkillAction,
				ETriggerEvent::Started,
				this,
				&AMainCharacter::PlayerSkill
			);
			
			// Interact 바인딩
			InputComponents->BindAction(
				PC->InteractAction,
				ETriggerEvent::Triggered,
				this,
				&AMainCharacter::PlayerInteract
			);
			
			// OpenInventory 바인딩
			InputComponents->BindAction(
				PC->OpenInventoryAction,
				ETriggerEvent::Started,
				this,
				&AMainCharacter::PlayerOpenInventory
			);
			
			// StartReload 바인딩
			InputComponents->BindAction(
				PC->ReloadAction,
				ETriggerEvent::Started,
				this,
				&AMainCharacter::PlayerStartReload
			);

			// FinishReload 바인딩
			InputComponents->BindAction(
				PC->ReloadAction,
				ETriggerEvent::Started,
				this,
				&AMainCharacter::PlayerFinishReload
			);
			
			// EscMenu 바인딩
			InputComponents->BindAction(
				PC->EscMenuAction,
				ETriggerEvent::Started,
				this,
				&AMainCharacter::ToggleEscMenu
			);

		}
	}
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// bIsAming 으로 조준 상태 구분
	float TargetLength = bIsAiming ? AimingArmLength : NormalArmLength;
	float TargetFOV = bIsAiming ? AimingFOV : NormalFOV;
	FVector TargetOffset = bIsAiming ? AimingSpringArm : NormalSpringArm;
	
	
	// Length 보간
	float NewArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		TargetLength,
		DeltaTime,
		CameraInterpSpeed
	);
	
	// 카메라 FOV 보간
	float NewCameraFOV = FMath::FInterpTo(
		Camera->FieldOfView,
		TargetFOV,
		DeltaTime,
		CameraInterpSpeed
	);
	
	// Socket 보간
	FVector NewSocketOffSet = FMath::VInterpTo(
		SpringArm->SocketOffset,
		TargetOffset,
		DeltaTime,
		CameraInterpSpeed
	);
	
	
	SpringArm->TargetArmLength = NewArmLength;
	Camera->FieldOfView = NewCameraFOV;
	SpringArm->SocketOffset = NewSocketOffSet;
}

void AMainCharacter::PlayerMove(const FInputActionValue& value)
{	
	if (!Controller)	return;
	
	const FVector2D MoveInput = value.Get<FVector2D>();
	
	if (bIsAiming)
	{
		if (!FMath::IsNearlyZero(MoveInput.X))
		{
			AddMovementInput(GetActorForwardVector(), MoveInput.X);
		}
	
		if (!FMath::IsNearlyZero(MoveInput.Y))
		{
			AddMovementInput(GetActorRightVector(), MoveInput.Y);
		}
	}
	else
	{
		FRotator CharacterRotation = Controller->GetControlRotation();	// 카메라 기준 회전값
		FRotator YawRotation(0, CharacterRotation.Yaw, 0);	// Yaw 값만 사용
		
		FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);	// 카메라 기준 forwardVector
		FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);		// 카메라 기준 RightVector
		
		AddMovementInput(ForwardDir, MoveInput.X);
		AddMovementInput(RightDir, MoveInput.Y);
	}
}

void AMainCharacter::PlayerLook(const FInputActionValue& value)
{	
	const FVector2D LookInput = value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AMainCharacter::PlayerStartSprint(const FInputActionValue& value)
{
	if (bIsAiming) return;
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = TotalStatus.Speed * SprintMultiplier;
	}
}

void AMainCharacter::PlayerStopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = TotalStatus.Speed;
	}
}

void AMainCharacter::PlayerDodge(const FInputActionValue& value)
{
	if (CurrentState != EAnimState::None|| bIsDodge || GetCharacterMovement()->IsFalling())
	{
		return;
	}
	
	UCharacterAnimInstance* CharacterAnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (CharacterAnimInstance && CharacterAnimInstance->IsAnyMontagePlaying())
	{
		return;
	}
	
	//CurrentStamina -= DodgeCost;	// 소모 스테미나
	bIsDodge = true;
	bIsInvicible = true;
	
	// 입력 방향 벡터 가져오기
	FVector InputDirection = GetLastMovementInputVector();
	
	if (bIsAiming && !InputDirection.IsNearlyZero())
	{
		bUseControllerRotationYaw = false;	// 회피 중 카메라와 캐릭터 시점 고정 해제
		GetCharacterMovement()->bOrientRotationToMovement = false;
		
		FRotator Direction = InputDirection.Rotation();
		Direction.Pitch = 0.0f;
		Direction.Roll = 0.0f;
		
		SetActorRotation(Direction);
	}
		
	// AnimMontage 재생
	PlayAnimMotageByState(EAnimState::Dodge);
}

void AMainCharacter::UpdateRotationState()
{
	if (bIsAiming || bIsFire)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;
	}
}

void AMainCharacter::ResetFireState()
{
	bIsFire = false;
	
	UpdateRotationState();
}

void AMainCharacter::PlayerAim(const FInputActionValue& value)
{
	if (bIsDodge) return;
	PrimaryActorTick.bCanEverTick = true;	// 보간을 위한 Tick On
	bIsAiming = true;
	// GetCharacterMovement()->bOrientRotationToMovement = false;
	// bUseControllerRotationYaw = true;		// 카메라와 캐릭터 방향 분리 
	UpdateRotationState();
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = TotalStatus.Speed * AimMultiplier;	// 줌 하는 동안 이동속도 감소
	}
}

void AMainCharacter::PlayerAimFinished(const FInputActionValue& value)
{
	PrimaryActorTick.bCanEverTick = false;	// Tick Off
	bIsAiming = false;
	// GetCharacterMovement()->bOrientRotationToMovement = true;
	// bUseControllerRotationYaw = false;		// 카메라와 캐릭터 방향 분리해제 
	UpdateRotationState();
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = TotalStatus.Speed;	
	}
}

void AMainCharacter::PlayerFire(const FInputActionValue& value)
{
	bIsFireButtonPressed = true;
	
	if (CombatComponent == nullptr || EquipmentComponent->CurrentWeapon == nullptr)	return;
	
	if (bIsDodge || bIsReload || IsFalling())
	{
		return;
	}
	
	// FireRate 0일 때 방어코드
	float FireRate = EquipmentComponent->CurrentWeapon->FireInterval;
	if (FireRate <= 0.0f) return;
	
	Fire();
	
	UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	GetWorldTimerManager().SetTimer(
		FireTimerHandle, 
		this, 
		&AMainCharacter::Fire,
		FireRate, 
		true
	);
}

void AMainCharacter::FinishFire(const FInputActionValue& value)
{
	bIsFireButtonPressed = false;
		
	if (CombatComponent == nullptr || EquipmentComponent->CurrentWeapon == nullptr)	return;
	
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	CombatComponent->StopFire();
	bIsFire = false;
}

void AMainCharacter::PlayerSkill(const FInputActionValue& value)
{
	if (CurrentState != EAnimState::None) return;
	
	UCharacterAnimInstance* CharacterAnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance()); 
		
	if (CharacterAnimInstance && !CharacterAnimInstance->IsAnyMontagePlaying())
	{
		PlayAnimMotageByState(EAnimState::Skill);
	}
	
}

void AMainCharacter::PlayerInteract(const FInputActionValue& value)
{
	if (CurrentInteractable)
	{
		CurrentInteractable->Interact(this);
	}
}

void AMainCharacter::SetCurrentInteractable(IInteractableInterface* Interactable)
{
	CurrentInteractable = Interactable;
}

IInteractableInterface* AMainCharacter::GetCurrentInteractable() const
{
	return CurrentInteractable;
}

void AMainCharacter::PlayerOpenInventory(const FInputActionValue& value)
{
	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		UIMgr->Toggle(UITags::Inventory);
	}
}

void AMainCharacter::PlayerStartReload(const FInputActionValue& value)
{
	if (bIsReload || bIsDodge) return;
	
	if (CombatComponent == nullptr || EquipmentComponent->CurrentWeapon == nullptr) return;
	
	int CurrentAmmo = CombatComponent->GetCurrentAmmo();
	int MaxAmmo = EquipmentComponent->CurrentWeapon->MaxAmmo;
	
	if (CurrentAmmo >= MaxAmmo) return;
	
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	
	Reload();
	// bIsReload = true;
	// GetWorldTimerManager().SetTimer(
	// ReloadTimerHandle, 
	// this, 
	// &AMainCharacter::Reload,
	// EquipmentComponent->CurrentWeapon->ReloadTime, 
	// false
	// );
	
}

void AMainCharacter::PlayerFinishReload(const FInputActionValue& value)
{
	//bIsReload = false;
}

void AMainCharacter::ToggleEscMenu(const FInputActionValue& value)
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Toggle(UITags::EscMenu);
	}
}

// 주현 : SoulGem 장착할 때마다 SoulGem의 스탯들을 모아서 StatusComponent에 재적용.
void AMainCharacter::HandleEquipmentChanged()
{
	TArray<FStatusModifier> Modifiers;
	
	UE_LOG(LogTemp, Warning, TEXT("Equipment Changed"));
	
	EquipmentComponent->CollectStatusModifiers(Modifiers);
	StatusComponent->UpdateTotalStat();
	//StatusComponent->CalculateStatusFromModifiers(Modifiers);
}

const FCharacterStat& AMainCharacter::GetTotalStatus() const
{
	return TotalStatus;
}

void AMainCharacter::SetTotalStatus(const FCharacterStat& NewStatus)
{
	TotalStatus = NewStatus;
	
	//TODO : 만약 UI(체력바, 스탯창)를 업데이트해야 한다면 여기서 Delegate를 호출
	//OnTotalStatChanged.Broadcast(TotalStatus);
}

int32 AMainCharacter::GetGold()
{
	return Gold;
}

void AMainCharacter::AddGold(int32 Amount)
{
	Gold += Amount;
}

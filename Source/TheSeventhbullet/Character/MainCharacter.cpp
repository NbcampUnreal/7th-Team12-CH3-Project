#include "MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainPlayerController.h"
#include "PlayerSkill.h"
#include "Animation/CharacterAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Component/CombatComponent.h" // 주현 : CombatComponent
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TheSeventhbullet/Weapon/WeaponBase.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MaxHP = 100;
	CurrentHP = MaxHP;
	MaxStamina = 100;	
	CurrentStamina = MaxStamina;
	DodgeCost = 20;
	DodgeDistance = 3000.0f;
	MaxSpeed = 600.0f;
	SprintMultifier = 1.5f;
	AimSpeed = 400.0f;
	NormalArmLength = 300.0f; 
	AimingArmLength = 0.0f;
	NormalSpringArm = FVector(0.0f, 25.0f, 0.0f);
	AimingSpringArm = FVector(0.0f, 25.0f, 35.0f);
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
	
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	
	bIsDodge = false;
	
	// 주현 : CombatComponent 초기화
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (AMainPlayerController* PC = Cast<AMainPlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PC->InputMappingContext, 0);
		}
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
				ETriggerEvent::Triggered,
				this,
				&AMainCharacter::PlayerDodge
			);
			
			// Dodge 바인딩
			InputComponents->BindAction(
				PC->DodgeAction,
				ETriggerEvent::Completed,
				this,
				&AMainCharacter::PlayerDodgeFinished
			);
			
			// Aim 바인딩
			InputComponents->BindAction(
				PC->AimAction,
				ETriggerEvent::Triggered,
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
				ETriggerEvent::Triggered,
				this,
				&AMainCharacter::PlayerOpenInventory
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
	FVector TargetOffset = bIsAiming ? AimingSpringArm : NormalSpringArm;
	
	// Length 보간
	float NewArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		TargetLength,
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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxSpeed * SprintMultifier;
	}
}

void AMainCharacter::PlayerStopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	}
}

void AMainCharacter::PlayerDodge(const FInputActionValue& value)
{
	if (bIsDodge || CurrentStamina < DodgeCost)
	{
		return;
	}
	
	//CurrentStamina -= DodgeCost;	// 소모 스테미나
	bIsDodge = true;
	
	// 캐릭터 방향 가져오기
	FVector DodgeDirection = GetActorForwardVector();
	
	// 입력 방향 벡터 가져오기
	FVector InputDirection = GetLastMovementInputVector();
	
	if (!InputDirection.IsNearlyZero())
	{
		DodgeDirection = InputDirection.GetSafeNormal();
	}
	
	LaunchCharacter(DodgeDirection*DodgeDistance, true, false);
}

void AMainCharacter::PlayerDodgeFinished(const FInputActionValue& value)
{
	bIsDodge = false;
}

void AMainCharacter::PlayerAim(const FInputActionValue& value)
{
	PrimaryActorTick.bCanEverTick = true;	// 보간을 위한 Tick On
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;		// 카메라와 캐릭터 방향 분리 
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = AimSpeed;	// 줌 하는 동안 이동속도 감소
	}
	
	bIsAiming = true;
	 
}

void AMainCharacter::PlayerAimFinished(const FInputActionValue& value)
{
	PrimaryActorTick.bCanEverTick = false;	// Tick Off
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;		// 카메라와 캐릭터 방향 분리해제 
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;	
	}
	
	bIsAiming = false;
}

void AMainCharacter::PlayerFire(const FInputActionValue& value)
{
	if (!CurrentWeapon)
	{
		return;
	}
	CombatComponent->StartFire();
}

void AMainCharacter::FinishFire(const FInputActionValue& value)
{
	if (!CurrentWeapon)
	{
		return;
	}
	CombatComponent->StopFire();
}

void AMainCharacter::PlayerSkill(const FInputActionValue& value)
{
	if (SkillMontage == nullptr) return;
	
	UCharacterAnimInstance* CharacterAnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance()); 
	
	if (CharacterAnimInstance)
	{
		if (!CharacterAnimInstance->Montage_IsPlaying(SkillMontage))
		{
			CharacterAnimInstance->Montage_Play(SkillMontage, 1.0f);
		}
	}
	
}

void AMainCharacter::PlayerInteract(const FInputActionValue& value)
{
}

void AMainCharacter::PlayerOpenInventory(const FInputActionValue& value)
{
}


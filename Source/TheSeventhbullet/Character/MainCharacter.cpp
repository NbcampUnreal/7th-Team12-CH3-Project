#include "MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainPlayerController.h"
#include "PlayerSkill.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
	NormalArmLength = 300.0f; 
	AimingArmLength = 0.0f;
	NormalSpringArm = FVector(0.0f, 25.0f, 0.0f);
	AimingSpringArm = FVector(0.0f, 25.0f, 35.0f);
	CameraInterpSpeed = 15.0f;
	MuzzleOffset = FVector(300.0f, 0.0f, 0.0f);
	HandSocketName = FName("MuzzleSocket");
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation= true;
	SpringArm->TargetArmLength = NormalArmLength;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
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
				ETriggerEvent::Triggered,
				this,
				&AMainCharacter::PlayerFire
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
	
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
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
	//bUseControllerRotationYaw = false;		// 카메라와 캐릭터 방향 분리 | 추후 에니메이션 넣고 활성화
	bIsAiming = true;
	// 줌 하는 동안 이동속도 감소 / 시야에 맞춰 캐릭터 정면 고정 
}

void AMainCharacter::PlayerAimFinished(const FInputActionValue& value)
{
	PrimaryActorTick.bCanEverTick = false;	// Tick Off
	//bUseControllerRotationYaw = false;		// 카메라와 캐릭터 방향 분리해제 | 추후 에니메이션 넣고 활성화
	bIsAiming = false;
}

void AMainCharacter::PlayerFire(const FInputActionValue& value)
{
}

void AMainCharacter::PlayerSkill(const FInputActionValue& value)
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
				
				
				SpawnRotation = GetControlRotation();	// 카메라가 바라보는 방향으로 던짐 
				//SpawnRotation = GetMesh()->GetSocketRotation(HandSocketName);
				
			}
			else
			{
				SpawnRotation = GetControlRotation();	// 카메라 회전
				SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);	// 캐릭터 위치 + 앞쪽 오프셋
			}
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;	// 누가 던졌는지 기록 추후 데미지 판정에 씀
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
			World->SpawnActor<APlayerSkill>(PlayerSkillClass, SpawnLocation, SpawnRotation, SpawnParams);
		}
	}
}

void AMainCharacter::PlayerInteract(const FInputActionValue& value)
{
}

void AMainCharacter::PlayerOpenInventory(const FInputActionValue& value)
{
}


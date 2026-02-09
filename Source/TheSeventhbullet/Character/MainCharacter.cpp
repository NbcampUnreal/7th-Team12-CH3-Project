#include "MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MaxHP = 100;
	CurrentHP = MaxHP;
	MaxStamina = 100;	
	CurrentStamina = MaxStamina;
	DodgeCost = 20;
	DodgeDistance = 3000.0f;
	MaxSpeed = 600.0f;
	SprintMultifier = 1.5f;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation= true;
	SpringArm->TargetArmLength = 300.0f;
	
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
			
			// Fire 바인딩
			InputComponents->BindAction(
				PC->FireAction,
				ETriggerEvent::Triggered,
				this,
				&AMainCharacter::PlayerFire
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
	// 카메라의 시야변경 (Spring arm 위치 변경)
	// 
}

void AMainCharacter::PlayerAimFinished(const FInputActionValue& value)
{
}

void AMainCharacter::PlayerFire(const FInputActionValue& value)
{
}

void AMainCharacter::PlayerInteract(const FInputActionValue& value)
{
}

void AMainCharacter::PlayerOpenInventory(const FInputActionValue& value)
{
}


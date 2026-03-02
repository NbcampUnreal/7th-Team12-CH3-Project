// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "Character/MainCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Character/Component/CombatComponent.h"
#include "Character/Component/EquipmentComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (OwnerCharacter == nullptr) return;
	
	// 이동 속도 계산
	FVector Velocity = OwnerCharacter->GetVelocity();	// 캐릭터의 현재 속도
	Velocity.Z = 0.0f;
	MovementSpeed = Velocity.Size();
	
	// 캐릭터 회전값 계산
	FRotator CharacterRotation = OwnerCharacter->GetBaseAimRotation();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, CharacterRotation);
	
	// 공중 체공 상태 계산
	UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement();
	if (MovementComp)
	{
		bIsFalling = MovementComp->IsFalling();
	}
	
	
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OwnerCharacter);
	if (MainCharacter)
	{
		
		if(MainCharacter->EquipmentComponent->CurrentWeapon)
		{
			EWeaponTypes WeaponType = MainCharacter->EquipmentComponent->CurrentWeapon->WeaponType;

			switch (WeaponType)
			{
			case EWeaponTypes::HandGun:
				CurrentWeaponType = EAnimWeaponType::HandGun;
				break;
			case EWeaponTypes::AssaultRifle:
				CurrentWeaponType = EAnimWeaponType::AssaultRifle;
				break;
			case EWeaponTypes::ShotGun:
				CurrentWeaponType = EAnimWeaponType::ShotGun;
				break;
			}
		}
		else
		{
			CurrentWeaponType = EAnimWeaponType::None;
		}
		
		// 회피 상태
		bIsDodging = MainCharacter->IsDodge();
		
		// 조준 상태
		bIsAiming = MainCharacter->IsAiming();
		
		// 사격 상태
		bIsFiring = MainCharacter->IsFiring();
		
		// Pitch 값 가져오기
		FRotator AimRotation = MainCharacter->GetBaseAimRotation();
		FRotator ActorRotation = MainCharacter->GetActorRotation();
		
		FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, ActorRotation);
		AimPitch = FMath::FInterpTo(AimPitch, DeltaRotation.Pitch, DeltaSeconds, 15.0f);
	}
}

void UCharacterAnimInstance::AnimNotify_Throw()
{
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner()))
	{
		MainCharacter->ThrowGrenade();
	}
}

void UCharacterAnimInstance::AnimNotify_EndSkill()
{
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner()))
	{
		MainCharacter->ShowWeaponMesh();
	}
}

void UCharacterAnimInstance::AnimNotify_RefillAmmo()
{
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner()))
	{
		MainCharacter->CombatComponent->Reload();
	}
}

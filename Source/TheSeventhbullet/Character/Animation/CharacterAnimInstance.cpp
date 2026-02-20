// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "Character/MainCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

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
	
	// 회피 상태 계산
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OwnerCharacter);
	if (MainCharacter)
	{
		bIsDodging = MainCharacter->IsDodge();
	}
	
	// 조준 상태 계산
	if (MainCharacter)
	{
		bIsAiming = MainCharacter->IsAiming();
	}
}

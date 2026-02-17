// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

void UEnemyAnimInstance::SetEnemyBlendSpace(UBlendSpace* BlendSpace)
{
	EnemyBlendSpace=BlendSpace;
}

UBlendSpace* UEnemyAnimInstance::GetEnemyBlendSpace()
{
	return EnemyBlendSpace;
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	Speed = Pawn->GetVelocity().Size();
}
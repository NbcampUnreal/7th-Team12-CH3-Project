// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyComboMoveNotifyState.h"

#include "Enemy/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyComboMoveNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}
	OwnerEnemyBase = Cast<AEnemyBase>(Owner);
	if (OwnerEnemyBase == nullptr)
	{
		return;
	}
	OwnerEnemyBase->SetAnimRootMotionTranslationScale(0.f);
}

void UEnemyComboMoveNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	FHitResult Hit;
	if (OwnerEnemyBase==nullptr)
	{
		return;
	}
	if (bIsForwardDirection)
	{

		MoveVector=OwnerEnemyBase->GetActorForwardVector();
	}
	else if (bIsZDirection)
	{
		UCharacterMovementComponent* EnemyCharacterMovementComponent=OwnerEnemyBase->GetCharacterMovement();
		if (EnemyCharacterMovementComponent==nullptr)
		{
			return;
		}
		MoveVector=FVector(0,0,1.0f);
	}
	MoveVector*=DashSpeed*FrameDeltaTime;
	OwnerEnemyBase->AddActorWorldOffset(MoveVector, true, &Hit,ETeleportType::None);
}	

void UEnemyComboMoveNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyComboMoveNotifyState.h"

#include "Enemy/EnemyBase.h"

void UEnemyComboMoveNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
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
	float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	
	FHitResult Hit;
	if (OwnerEnemyBase==nullptr)return;
	MoveVector=OwnerEnemyBase->GetActorForwardVector()*DashSpeed*FrameDeltaTime;
	OwnerEnemyBase->AddActorWorldOffset(MoveVector, true, &Hit,ETeleportType::None);
}

void UEnemyComboMoveNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
}

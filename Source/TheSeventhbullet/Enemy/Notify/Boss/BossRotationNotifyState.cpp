// Fill out your copyright notice in the Description page of Project Settings.


#include "BossRotationNotifyState.h"

#include "Enemy/EnemyBase.h"

void UBossRotationNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}
}

void UBossRotationNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	
	Owner->SetActorRotation(Owner->GetActorRotation()+FRotator(0,36.f*FrameDeltaTime*(bIsPlusDirection?1:-1),0));
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnHitGroundNotifyState.h"
#include "Enemy/Boss/BossPatternNeeds/HitGroundActor.h"

void USpawnHitGroundNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration)
{
	if (MeshComp==nullptr || !MeshComp->GetWorld()->IsGameWorld())
	{
		return;
	}
	SpawnLocation=MeshComp->GetOwner()->GetActorLocation();
	SpawnLocation=FVector(SpawnLocation.X,SpawnLocation.Y,-11990.0f);
	if (MeshComp->GetWorld()==nullptr)
	{
		return;
	}
	SpawnedHitGroundActor=MeshComp->GetWorld()->SpawnActor<AHitGroundActor>(HitGroundActor,SpawnLocation,FRotator::ZeroRotator);
	SpawnedHitGroundActor->SetActorScale3D((FVector(HitGroundScale,HitGroundScale,HitGroundScale)));
}


void USpawnHitGroundNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp==nullptr || !MeshComp->GetWorld()->IsGameWorld())
	{
		return;
	}
	if (SpawnedHitGroundActor==nullptr)
	{
		return;
	}
	SpawnedHitGroundActor->Explosion();
}

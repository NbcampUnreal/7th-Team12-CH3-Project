// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectilePoolManager.h"

#include "Enemy/EnemyBase.h"
#include "Enemy/Projectile/ProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UProjectilePoolManager::Initialize(FSubsystemCollectionBase& SubsystemCollectionBase)
{
	Super::Initialize(SubsystemCollectionBase);
	if (GetWorld()==nullptr||!GetWorld()->IsGameWorld())
	{
		return;
	}
	
	ProjectileTransform.SetLocation(FVector(-2000.0f, -2000.0f, 0.0f));
	
	for (int32 i=0;i<50;i++)
	{
		TObjectPtr<AActor> Projectile=GetWorld()->SpawnActor<AProjectileActor>();
		Projectile->SetActorTransform(ProjectileTransform);
		Projectile->SetActorHiddenInGame(true);
		ProjectilePool.Add(Projectile);
	}
	
}

TObjectPtr<AActor> UProjectilePoolManager::GetProjectile()
{
	for (int32 i=0;i<50;i++)
	{
		if (ProjectilePool[i]->IsHidden())
		{
			return ProjectilePool[i];
		}
	}
	return nullptr;
}

void UProjectilePoolManager::ReturnToPool(TObjectPtr<AActor> Projectile)
{
	TObjectPtr<AProjectileActor> ProjectileActor = Cast<AProjectileActor>(Projectile);
	if (ProjectileActor == nullptr)
	{
		return;
	}
	ProjectileActor->SetActiveAndCollision(false);
	ProjectileActor->SetActorTransform(ProjectileTransform);
	
}


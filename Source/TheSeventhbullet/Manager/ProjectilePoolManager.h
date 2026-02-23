// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectilePoolManager.generated.h"

/**
 * WorldSubsystem으로, 발사체의 오브젝트 풀을 관리합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UProjectilePoolManager : public UWorldSubsystem
{
	GENERATED_BODY()

public: 
	virtual void Initialize(FSubsystemCollectionBase&);
	
	TObjectPtr<AActor> GetProjectile();
	
	void ReturnToPool(TObjectPtr<AActor> Projectile);
	
private:
	FTransform ProjectileTransform;
	
	TArray<TObjectPtr<AActor>> ProjectilePool;
};

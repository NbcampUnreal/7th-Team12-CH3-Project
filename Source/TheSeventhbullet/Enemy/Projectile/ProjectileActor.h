// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"


class UProjectileMovementComponent;
class USphereComponent;
class AEnemyBase;
/**
 * 소환한 대상의 정면 방향으로 날아가며 Sphere Trace를 진행해, 맞으면 데미지를 입힙니다.
 */
UCLASS()
class THESEVENTHBULLET_API AProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileActor();
	UFUNCTION()
	void OnProjectileHit(
		UPrimitiveComponent* HitComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		FVector NormalImpulse, 
		const FHitResult& Hit
		);
	
	
	void SetEnemySetting(TObjectPtr<AEnemyBase> InEnemy);
	
	void SetActiveAndCollision(bool InActive);
	
	void LifeTimeEnd();

private:
	UPROPERTY(VisibleAnywhere)
	float AttackPoint=0.0f;	
	
	UPROPERTY(VisibleAnywhere)
	float ProjectileSpeed=1000.0f;	
	
	UPROPERTY(VisibleAnywhere)
	bool bIsHoming=false;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ProjectileStaticMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere)
    TObjectPtr<AEnemyBase> Enemy;
	
	
	
	FTimerHandle TimerHandle;
	
	
};

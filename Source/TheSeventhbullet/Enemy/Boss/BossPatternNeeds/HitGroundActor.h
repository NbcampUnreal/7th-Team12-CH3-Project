// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitGroundActor.generated.h"

class USphereComponent;

UCLASS()
class THESEVENTHBULLET_API AHitGroundActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHitGroundActor();
	
	//외부에서 호출
	UFUNCTION()
	void Explosion();
	UFUNCTION()
	void SetDamage(float InDamage);
	
protected:
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category=Setting)
	float ExplosionDamage=20.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category=Setting)
	float LaunchPower=10.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category=Setting)
	TObjectPtr<USceneComponent>  Scene;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category=Setting)
	TObjectPtr<USphereComponent> ExplosionCollision;
	
};

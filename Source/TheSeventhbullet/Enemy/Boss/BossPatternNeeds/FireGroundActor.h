// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireGroundActor.generated.h"

class UBoxComponent;

UCLASS()
class THESEVENTHBULLET_API AFireGroundActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFireGroundActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collision")
	TObjectPtr<UBoxComponent> CollisionComponent;
	
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

};

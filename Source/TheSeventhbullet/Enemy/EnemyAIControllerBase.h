// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIControllerBase.generated.h"


UCLASS()
class THESEVENTHBULLET_API AEnemyAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyAIControllerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> EnemyBehaviorTree;
	TObjectPtr<UBlackboardComponent> BBComp;
	
	
	virtual void OnPossess(APawn* InPawn) override;
	UFUNCTION()
	void HitEvent();
	UFUNCTION()
	void DeadEvent();
	
private:
	FTimerHandle HitTimerHandle;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "EnemyAIControllerBase.generated.h"

/**
 * 적 AIController입니다.
 */
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
	
	UFUNCTION()
	void SetBT(UBehaviorTree* EnemyBT);

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> EnemyBehaviorTree;
	TObjectPtr<UBlackboardComponent> BBComp;
	
	//bool BB 키(FName)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FName bIsDeadKey="bIsDead";
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FName bIsHitKey="bIsHit";
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="BehaviorTree|BBKey")
	FName bIsHeadHitKey="bIsHeadHit";
	
	
	//AI퍼셉션
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	
	
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	void HitEvent();
	UFUNCTION()
	void DeadEvent();
	UFUNCTION()
	void ResetEvent();
	UFUNCTION()
	void HeadHitEvent();
	UFUNCTION()
	void SetAI(UBehaviorTree* ParamBT,float AttackRadius);
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;
};

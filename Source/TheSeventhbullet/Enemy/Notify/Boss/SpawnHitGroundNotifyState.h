// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SpawnHitGroundNotifyState.generated.h"

class AHitGroundActor;
/**
 * HitGround를 생성합니다.
 */
UCLASS()
class THESEVENTHBULLET_API USpawnHitGroundNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	
protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category=Data)
	TSubclassOf<AHitGroundActor> HitGroundActor;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category=Data)
	float HitGroundScale=1.0f;
	
	FVector SpawnLocation=FVector::ZeroVector;
	
	TObjectPtr<AHitGroundActor> SpawnedHitGroundActor;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EnemyComboMoveNotifyState.generated.h"

class AEnemyBase;
/**
 * 콤보 공격중 대시합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UEnemyComboMoveNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime )override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Speed")
	float DashSpeed=2.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Speed")
	FVector MoveVector;
private:
	UPROPERTY()
	TObjectPtr<AActor> Owner;
	//적 액터를 캐릭터로 형변환
	UPROPERTY()
	TObjectPtr<AEnemyBase> OwnerEnemyBase;
	
};

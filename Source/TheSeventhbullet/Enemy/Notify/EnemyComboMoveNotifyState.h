// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EnemyComboMoveNotifyState.generated.h"

class AEnemyBase;
/**
 * 애니메이션 몽타주 플레이 중에  대시합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UEnemyComboMoveNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Speed")
	float DashSpeed=2.0f;
	
	//디폴트 : Forward로 대시하는 경우
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Direction")
	bool bIsForwardDirection=true;
	//Z방향으로 대시하는 경우
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Direction")
	bool bIsZDirection=false;
	
	UPROPERTY()
	FVector MoveVector;
	

	
	
private:
	UPROPERTY()
	TObjectPtr<AActor> Owner;
	//적 액터를 캐릭터로 형변환
	UPROPERTY()
	TObjectPtr<AEnemyBase> OwnerEnemyBase;
	

	
};

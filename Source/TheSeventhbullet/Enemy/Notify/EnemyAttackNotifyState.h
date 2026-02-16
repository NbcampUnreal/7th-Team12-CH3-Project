// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EnemyAttackNotifyState.generated.h"

/**
 *  공격하는 동안 소켓의 경로대로 SphereTrace를 진행하여 공격판정을 진행하는 노티파이 스테이트입니다.
 */

class AMainCharacter;
class AEnemyBase;
UCLASS()
class THESEVENTHBULLET_API UEnemyAttackNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	UEnemyAttackNotifyState();
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime )override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Attack")
	float TraceRadius=50.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Attack")
	bool bShowDebug;
	
	
private:
	FVector PresentAttackSocketLocation=FVector::ZeroVector;
	FVector CurrentAttackSocketLocation=FVector::ZeroVector;
	//공격을 진행하는 적 액터
	TObjectPtr<AActor> Owner;
	//적 액터를 캐릭터로 형변환
	TObjectPtr<AEnemyBase> OwnerEnemyBase;
	//맞은 액터(대부분 플레이어 캐릭터)
	TObjectPtr<AActor> HittedActor;
	//맞은 액터를 메인 캐릭터로 형변환
	TObjectPtr<AMainCharacter> HittedCharacter;
	
	TArray<TObjectPtr<AActor>> HittedCharacterArray;
	
	
};

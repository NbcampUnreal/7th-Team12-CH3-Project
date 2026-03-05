// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BossRazorNotifyState.generated.h"

class AMainCharacter;
class AEnemyBase;
class AEmitter;
/**
 *  보스 몬스터의 레이저 공격 판정 및 파티클 생성을 진행합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBossRazorNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Attack")
	float RazorTraceRadius=50.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Attack")
	float RazorDistance=5000.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Attack")
	bool bShowDebug=false;
		
	//XY방향으로 날리는 힘
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Attack")
	float RazorLaunchPowerXY;
	//Z방향으로 날리는 힘
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Attack")
	float RazorLaunchPowerZ;
	//레이저 파티클
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Attack")
	UParticleSystem* RazorParticleSystem;

	
private:
	//공격을 진행하는 적 액터
	UPROPERTY()
	TObjectPtr<AActor> Owner=nullptr;
	//적 액터를 캐릭터로 형변환
	UPROPERTY()
	TObjectPtr<AEnemyBase> OwnerEnemyBase;
	//맞은 액터(대부분 플레이어 캐릭터)
	UPROPERTY()
	TObjectPtr<AActor> HittedActor;
	
	//맞은 액터를 메인 캐릭터로 형변환
	UPROPERTY()
	TObjectPtr<AMainCharacter> HittedCharacter;
	
	FVector KnockBackDirection=FVector::ZeroVector;
	//파티클 소환을 위한 이미터
	TObjectPtr<AEmitter> RazorParticleActor;
};

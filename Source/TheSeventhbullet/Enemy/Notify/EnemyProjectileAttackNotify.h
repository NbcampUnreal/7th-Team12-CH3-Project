// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EnemyProjectileAttackNotify.generated.h"

/**
 * 해당 노티파이 시점에서 발사체를 스폰합니다.
 */
class AProjectileActor;
UCLASS()
class THESEVENTHBULLET_API UEnemyProjectileAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	UEnemyProjectileAttackNotify();
	
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
							const FAnimNotifyEventReference& EventReference);

protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Projectile)
	TObjectPtr<AProjectileActor> ProjectileActor;

};

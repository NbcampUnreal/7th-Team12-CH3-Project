// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyDataAsset.generated.h"


/**
 * EnemyBase를 값을 주입합니다.
 */

UCLASS()
class THESEVENTHBULLET_API UEnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GetItemType(), GetFName());
	}
	
	virtual FPrimaryAssetType GetItemType() const
	{
		return FPrimaryAssetType("Enemy");
	}
	
public:
	//ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	FString MonsterID;
	//최대체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	float MaxHealth;
	//방어력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	float ArmorPoint;
	//공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	float AttackPoint;
	//원거리 몬스터인가?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	bool bIsLongRange;
	
	
	//스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	//애님몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TMap<FName,TSoftObjectPtr<UAnimMontage>> AnimMontages;
	//Walk/Run 블렌드 스페이스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UBlendSpace> EnemyBlendSpace;
	//적의 ABP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TSoftClassPtr<UAnimInstance> EnemyAnimationBlueprint;
	//사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TMap<FName,TSoftObjectPtr<USoundCue>> SoundCues;
	//발사체의 스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<USkeletalMesh> ProjectileSkeletalMesh;
	//발사체의 애님몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UAnimMontage> ProjectileAnimMontage;
	//히트 파티클
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UParticleSystem> HitParticle;
	//헤드샷 파티클
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UParticleSystem> HeadShotParticle;
	//BehaviorTree
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UBehaviorTree> EnemyBT;
};

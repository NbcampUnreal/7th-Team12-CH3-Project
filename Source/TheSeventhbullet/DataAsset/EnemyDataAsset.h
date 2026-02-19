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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Status")
	float MaxHealth;
	//방어력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Status")
	float ArmorPoint;
	//공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Status")
	float AttackPoint;
	//공격 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Status")
	float AttackRadius;
	//발사체의 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Status")
	float ProjectileSpeed;
	//발사체의 호밍 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Status")
	bool bIsHoming;
	
	
	//스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	//애님몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TMap<FName,TSoftObjectPtr<UAnimMontage>> AnimMontages;
	//Walk/Run 블렌드 스페이스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UBlendSpace> EnemyBlendSpace;
	//적의 ABP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TSoftClassPtr<UAnimInstance> EnemyAnimationBlueprint;
	//사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TMap<FName,TSoftObjectPtr<USoundCue>> SoundCues;
	//발사체의 스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UStaticMesh> ProjectileStaticMesh;

	//히트 파티클
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UParticleSystem> HitParticle;
	//헤드샷 파티클
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UParticleSystem> HeadShotParticle;
	//BehaviorTree
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UBehaviorTree> EnemyBT;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TSoftObjectPtr<UMaterial> EnemyMaterial;
	
	//AIControllerClass
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AssetBundles = "Enemy"))
	TSoftClassPtr<AAIController> AIControllerClass;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TheSeventhbullet/Enemy/Boss/BossPatternInfoDataAsset.h"
#include "BossPatternDataAsset.generated.h"

/**
 * Boss의 패턴 정보를 외부에서 주입하는 PDA입니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBossPatternDataAsset : public UPrimaryDataAsset
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data", meta=(AssetBundles="Enemy"))
	TMap<FName,TSoftObjectPtr<UBossPatternInfoDataAsset>> PatternMap;
};

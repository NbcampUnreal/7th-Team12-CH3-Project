// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"CoreMinimal.h"
#include"Engine/DataAsset.h"
#include"ItemDataAsset.generated.h"

UCLASS()
class THESEVENTHBULLET_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GetItemType(), GetFName());
	}
	
	virtual FPrimaryAssetType GetItemType() const
	{
		return FPrimaryAssetType("Item");
	}
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 MaxStackCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSoftClassPtr<AActor> PickupActorClass;
};

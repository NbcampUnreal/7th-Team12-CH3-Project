// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileStat.generated.h"

/**
 * 
 */
class THESEVENTHBULLET_API ProjectileStat
{
public:
	ProjectileStat();
	~ProjectileStat();
};

USTRUCT(Atomic,BlueprintType)
struct FProjectileStatus
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMesh> StaticMesh;
	UPROPERTY(VisibleAnywhere)
	float Speed;
	UPROPERTY(VisibleAnywhere)
	bool bIsHoming;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterial> Material;
};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileStat.generated.h"

/**
 * 발사체의 스탯을 담는 구조체 클래스입니다.
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
	//발사체의 Static Mesh
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMesh> StaticMesh=nullptr;
	//발사체의 속도
	UPROPERTY(VisibleAnywhere)
	float Speed=0.0f;
	//발사체의 호밍(추적) 여부
	UPROPERTY(VisibleAnywhere)
	bool bIsHoming=false;
	//발사체의 오버레이 머티리얼
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterial> Material=nullptr;
};


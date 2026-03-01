// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BossPatternInfoDataAsset.generated.h"

/**
 * 보스의 패턴 상세정보를 담는 DataAsset입니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBossPatternInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()
	//패턴의 애니메이션 몽타주 맵
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Data")
	TMap<FName,TObjectPtr<UAnimMontage>> PatternMontageMap;
	//패턴의 데미지 컨테이너
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Data")
	TArray<float> DamageArray;
	//패턴의 쿨타임
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Data")
	float CoolTime=0.0f;
	//패턴에 필요한 액터 컨테이너
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Data")
	TMap<FName,TObjectPtr<AActor>> PatternActorMap;
};

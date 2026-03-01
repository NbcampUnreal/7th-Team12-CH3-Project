// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "BossCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossEventSignnature);

class UBossPatternComponent;
UCLASS()
class THESEVENTHBULLET_API ABossCharacter : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossCharacter();

protected:
	TObjectPtr<UBossPatternComponent> BossPatternComponent;
	TObjectPtr<UBossPatternDataAsset> BossData;
	
	FOnBossEventSignnature OnBossEvent;
	
	virtual void SetPattern(UBossPatternDataAsset* PatternData) override;
	virtual void PlayPattern(FName PatternName) override;
	
};

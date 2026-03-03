// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPatternComponentInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossPatternEndSignnature);

class UBrainComponent;
class AEnemyBase;
class ULevelSequence;
class ULevelSequencePlayer;


UCLASS(ClassGroup=(Custom),Blueprintable, meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UBossPatternComponentInterface : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void BossMonsterPlayPattern() PURE_VIRTUAL (UBossPatternComponentInterface::BossMonsterPlayPattern,);
	
	FOnBossPatternEndSignnature OnBossPatternEndSignature;
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "LevelSequence")
	ULevelSequence* BossLevelSequence;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "LevelSequence")
	 ULevelSequencePlayer* BossSequencePlayer;
	
	AEnemyBase* BossEnemy;
	UBrainComponent* BossBrainComponent;
	
	
	UFUNCTION()
	virtual void OnBossSequenceFinishedDelegate()PURE_VIRTUAL (UBossPatternComponentInterface::OnBossSequenceFinishedDelegate,);;
};

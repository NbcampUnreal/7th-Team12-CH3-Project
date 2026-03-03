// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossPatternComponentInterface.h"
#include "MovieSceneObjectBindingID.h"
#include "BreakGroundBossPatternComponent.generated.h"

class ULevelSequence;
struct FMovieSceneBinding;
UCLASS(ClassGroup=(Custom),Blueprintable, meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UBreakGroundBossPatternComponent : public UBossPatternComponentInterface
{
	GENERATED_BODY()

public:
	virtual void BossMonsterPlayPattern() override;
	
protected:

	FMovieSceneObjectBindingID CharacterBinding;
	FMovieSceneObjectBindingID BossBinding;

	TArray<AActor*> CharacterActors;
	TArray<AActor*> BossActors;
	
	virtual void OnBossSequenceFinishedDelegate() override;
};

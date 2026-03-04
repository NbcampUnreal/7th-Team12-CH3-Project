// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossPatternComponentInterface.h"
#include "MovieSceneObjectBindingID.h"
#include "DeadBossPatternComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UDeadBossPatternComponent : public UBossPatternComponentInterface
{
	GENERATED_BODY()

public:
	virtual void BossMonsterPlayPattern() override;

protected:

	FMovieSceneObjectBindingID CharacterBinding;
	FMovieSceneObjectBindingID BossBinding;

	TArray<AActor*> CharacterActors;
	TArray<AActor*> BossActors;
	
	TArray<AActor*> BreakBossMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> BreakBossMapClass;
	
	virtual void OnBossSequenceFinishedDelegate() override;
	
};

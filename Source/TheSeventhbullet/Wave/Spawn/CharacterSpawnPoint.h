// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterSpawnPoint.generated.h"

UCLASS()
class THESEVENTHBULLET_API ACharacterSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ACharacterSpawnPoint();

protected:
	virtual void BeginPlay() override;

};

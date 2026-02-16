// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheSeventhbullet/Data/TableRowTypes.h"
#include "Spawner.generated.h"

class UBoxComponent;

UCLASS()
class THESEVENTHBULLET_API ASpawner : public AActor
{
	GENERATED_BODY()

public:
	ASpawner();

protected:
	virtual void BeginPlay() override;
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Scene = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> SpawnBox = nullptr;
	
	UFUNCTION()
	FVector GetRandomPointInVolume() const;
	
};

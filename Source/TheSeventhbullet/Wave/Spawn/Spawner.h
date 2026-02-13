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
	virtual void Tick(float DeltaTime) override;
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Scene = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> SpawnBox = nullptr;
	
	UFUNCTION()
	void InitializeSpawner(const FSpawnRowData& Data);
	
	UFUNCTION()
	void Spawn();
private:
	AActor* SpawnRandomMonster();
	FVector GetRandomPointInVolume() const;
	AActor* SpawnMonster(FName MonsterType);
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data", meta=(AllowPrivateAccess="true"))
	FSpawnRowData WaveData;
	
};

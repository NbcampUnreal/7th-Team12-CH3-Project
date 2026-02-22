// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamTrigger.generated.h"

class UBoxComponent;

UCLASS()
class THESEVENTHBULLET_API ALevelStreamTrigger : public AActor
{
	GENERATED_BODY()

public:
	ALevelStreamTrigger();
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	UBoxComponent* OverlapVolume;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStream")
	FName LevelToLoad;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStream")
	FName LevelToUnload;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlapComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TheSeventhbullet/Data/TableRowTypes.h"
#include "SoundManager.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	
	static USoundManager* Get(const UObject* WorldContext);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable)
	void PlaySoundUI(FName SoundID);
	
	UFUNCTION(BlueprintCallable)
	void PlaySoundAtLocation(FName SoundID, FVector Location);
	
	UFUNCTION(BlueprintCallable)
	void PlaySoundAttached(FName SoundID, USceneComponent* AttachToComponent);
	
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void SetCategoryVolume(USoundClass* SoundClass, float Volume);
private:
	FSoundData* GetSoundData(FName SoundID);
	
	UPROPERTY()
	TMap<FName, FSoundData> SoundCache;
};

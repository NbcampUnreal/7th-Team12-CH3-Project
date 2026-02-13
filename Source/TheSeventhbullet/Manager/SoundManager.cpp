// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"
#include "SyncDataManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "TheSeventhbullet/System/GameInstance/MainGameInstance.h"

USoundManager* USoundManager::Get(const UObject* WorldContext)
{
	UMainGameInstance* GameInstance = UMainGameInstance::Get(WorldContext);
	if (!GameInstance) return nullptr;
	
	return GameInstance->GetSubsystem<USoundManager>();
}

void USoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	USyncDataManager* Manager = USyncDataManager::Get(GetWorld());
	if (!Manager)
	{
		UE_LOG(LogTemp,Error,TEXT("[SoundManager] SyncDataManager Can't found"));
	}
	
	Manager->LoadAndCacheTable<FSoundData, FName>(
		TEXT("/Game/TheSeventhBullet/DataTable/DT_Sound"),
		SoundCache,
		[](const FSoundData* Row) {return Row->SoundName;}
		);
}

void USoundManager::PlaySoundUI(FName SoundID)
{
	FSoundData* Data = GetSoundData(SoundID);
	if (Data)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Data->SoundAsset, Data->VolumeMultiplier);
	}
}

void USoundManager::PlaySoundAtLocation(FName SoundID, FVector Location)
{
	FSoundData* Data = GetSoundData(SoundID);
	if (Data)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Data->SoundAsset, Location,Data->VolumeMultiplier);
	}
}

void USoundManager::PlaySoundAttached(FName SoundID, USceneComponent* AttachToComponent)
{
	FSoundData* Data = GetSoundData(SoundID);
	if (Data && AttachToComponent)
	{
		UGameplayStatics::SpawnSoundAttached(Data->SoundAsset, AttachToComponent, NAME_None,
			FVector::ZeroVector, EAttachLocation::KeepRelativeOffset,
			true, Data->VolumeMultiplier);
	}
}

void USoundManager::SetCategoryVolume(USoundClass* SoundClass, float Volume)
{
	if (SoundClass)
	{
		SoundClass->Properties.Volume = Volume;
	}
}

FSoundData* USoundManager::GetSoundData(FName SoundID)
{
	if (FSoundData* FoundData = SoundCache.Find(SoundID))
	{
		return FoundData;
	}
	return nullptr;
}

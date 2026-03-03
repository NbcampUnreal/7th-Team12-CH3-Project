// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"
#include "SyncDataManager.h"
#include "Components/AudioComponent.h"
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

void USoundManager::PlayBGM(FName SoundID, float FadeInTime, float FadeOutTime, bool bRestartIfSame)
{
	if (!bRestartIfSame && CurrentBGMComp && CurrentBGMID == SoundID && CurrentBGMComp->IsPlaying())
	{
		return;
	}

	// 기존 BGM 정리
	StopBGM(FadeOutTime);

	FSoundData* Data = GetSoundData(SoundID);
	if (!Data || !Data->SoundAsset) return;

	// SpawnSound2D는 UAudioComponent* 리턴 -> 이걸 저장해야 Stop 가능
	UAudioComponent* NewComp = UGameplayStatics::SpawnSound2D(
		GetWorld(),
		Data->SoundAsset,
		Data->VolumeMultiplier,
		1.0f,        // Pitch
		0.0f,        // StartTime
		nullptr,     // Concurrency
		true         // bPersistAcrossLevelTransition (레벨 이동해도 유지)
	);

	if (!NewComp) return;

	NewComp->bIsUISound = true;
	NewComp->bAutoDestroy = false;
	NewComp->SetVolumeMultiplier(1.f);
	NewComp->FadeIn(FadeInTime, Data->VolumeMultiplier);

	CurrentBGMComp = NewComp;
	CurrentBGMID = SoundID;
}

void USoundManager::StopBGM(float FadeOutTime)
{
	if (!CurrentBGMComp) return;

	if (FadeOutTime > 0.f)
	{
		CurrentBGMComp->FadeOut(FadeOutTime, 0.f);
	}
	else
	{
		CurrentBGMComp->Stop();
		CurrentBGMComp->DestroyComponent();
	}

	CurrentBGMComp = nullptr;
	CurrentBGMID = NAME_None;
}

FSoundData* USoundManager::GetSoundData(FName SoundID)
{
	if (FSoundData* FoundData = SoundCache.Find(SoundID))
	{
		return FoundData;
	}
	return nullptr;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"
#include "SyncDataManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "GameFramework/GameUserSettings.h"
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

	LoadAudioSettings();
}

void USoundManager::PlaySoundUI(FName SoundID)
{
	FSoundData* Data = GetSoundData(SoundID);
	if (Data)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Data->SoundAsset, Data->VolumeMultiplier * MasterVolume * SFXVolume);
	}
}

void USoundManager::PlaySoundAtLocation(FName SoundID, FVector Location)
{
	FSoundData* Data = GetSoundData(SoundID);
	if (Data)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Data->SoundAsset, Location, Data->VolumeMultiplier * MasterVolume * SFXVolume);
	}
}

void USoundManager::PlaySoundAttached(FName SoundID, USceneComponent* AttachToComponent)
{
	FSoundData* Data = GetSoundData(SoundID);
	if (Data && AttachToComponent)
	{
		UGameplayStatics::SpawnSoundAttached(Data->SoundAsset, AttachToComponent, NAME_None,
			FVector::ZeroVector, EAttachLocation::KeepRelativeOffset,
			true, Data->VolumeMultiplier * MasterVolume * SFXVolume);
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
	const float FinalBGMVolume = Data->VolumeMultiplier * MasterVolume * BGMVolume;
	UAudioComponent* NewComp = UGameplayStatics::SpawnSound2D(
		GetWorld(),
		Data->SoundAsset,
		FinalBGMVolume,
		1.0f,        // Pitch
		0.0f,        // StartTime
		nullptr,     // Concurrency
		true         // bPersistAcrossLevelTransition (레벨 이동해도 유지)
	);

	if (!NewComp) return;

	NewComp->bIsUISound = true;
	NewComp->bAutoDestroy = false;
	NewComp->SetVolumeMultiplier(FinalBGMVolume);
	NewComp->FadeIn(FadeInTime, FinalBGMVolume);

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

void USoundManager::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	UpdateBGMVolume();
}

void USoundManager::SetBGMVolume(float Volume)
{
	BGMVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	UpdateBGMVolume();
}

void USoundManager::SetSFXVolume(float Volume)
{
	SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void USoundManager::UpdateBGMVolume()
{
	if (CurrentBGMComp && CurrentBGMComp->IsPlaying())
	{
		FSoundData* Data = GetSoundData(CurrentBGMID);
		const float BaseVolume = Data ? Data->VolumeMultiplier : 1.0f;
		CurrentBGMComp->SetVolumeMultiplier(BaseVolume * MasterVolume * BGMVolume);
	}
}

void USoundManager::SaveAudioSettings()
{
	if (GConfig)
	{
		const FString Section = TEXT("/Script/TheSeventhbullet.AudioSettings");
		GConfig->SetFloat(*Section, TEXT("MasterVolume"), MasterVolume, GGameUserSettingsIni);
		GConfig->SetFloat(*Section, TEXT("BGMVolume"), BGMVolume, GGameUserSettingsIni);
		GConfig->SetFloat(*Section, TEXT("SFXVolume"), SFXVolume, GGameUserSettingsIni);
		GConfig->Flush(false, GGameUserSettingsIni);
	}
}

void USoundManager::LoadAudioSettings()
{
	if (GConfig)
	{
		const FString Section = TEXT("/Script/TheSeventhbullet.AudioSettings");
		GConfig->GetFloat(*Section, TEXT("MasterVolume"), MasterVolume, GGameUserSettingsIni);
		GConfig->GetFloat(*Section, TEXT("BGMVolume"), BGMVolume, GGameUserSettingsIni);
		GConfig->GetFloat(*Section, TEXT("SFXVolume"), SFXVolume, GGameUserSettingsIni);
	}
}

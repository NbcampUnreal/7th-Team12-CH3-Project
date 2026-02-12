// Fill out your copyright notice in the Description page of Project Settings.


#include"AsyncDataManager.h"
#include"Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

class UAssetManager;



UAsyncDataManager* UAsyncDataManager::Get(const UObject* WorldContextObject)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GI->GetSubsystem<UAsyncDataManager>();
	}
	return nullptr;
}

void UAsyncDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UAsyncDataManager::Deinitialize()
{
	//Flush Handle
	for (auto& Pair : LoadingHandles)
	{
		if (Pair.Value.IsValid())
		{
			Pair.Value->CancelHandle();
		}
	}
	LoadingHandles.Empty();
	
	for (auto& Handle : AssetsHandles)
	{
		if (Handle.IsValid())
		{
			Handle->CancelHandle();
		}
	}
	
	AssetsHandles.Empty();
	LoadedAssets.Empty();
	Super::Deinitialize();
}

void UAsyncDataManager::LoadAssetAsync(FPrimaryAssetId AssetID, FOnAssetLoaded OnLoaded)
{
	//Loaded Case
	if (TObjectPtr<UPrimaryDataAsset>* Found = LoadedAssets.Find(AssetID))
	{
		OnLoaded.ExecuteIfBound(*Found);
		return;
	}
	//Loading Case
	if (LoadingHandles.Contains(AssetID))
	{
		return;
	}
	
	UAssetManager& Manager = UAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = Manager.LoadPrimaryAsset(
		AssetID,
		TArray<FName>(),
		FStreamableDelegate::CreateLambda([this, AssetID, OnLoaded]()
		{
			UPrimaryDataAsset* Asset = Cast<UPrimaryDataAsset>(
				UAssetManager::Get().GetPrimaryAssetObject(AssetID)
			);

			if (Asset)
			{
				LoadedAssets.Add(AssetID, Asset);
			}

			LoadingHandles.Remove(AssetID);
			OnLoaded.ExecuteIfBound(Asset);
		})
	);
}

void UAsyncDataManager::LoadAssetsAsync(TArray<FPrimaryAssetId> AssetIDs, FOnAssetsLoaded OnAllLoaded)
{
	bool bIsAllLoaded = true;
	for (const FPrimaryAssetId& ID : AssetIDs)
	{
		if (!LoadedAssets.Contains(ID))
		{
			bIsAllLoaded = false;
			break;
		}
	}
	if (bIsAllLoaded)
	{
		OnAllLoaded.ExecuteIfBound();
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = Manager.LoadPrimaryAssets(
		AssetIDs,
		TArray<FName>(),
		FStreamableDelegate::CreateLambda([this, AssetIDs, OnAllLoaded]()
		{
			UAssetManager& Mgr = UAssetManager::Get();
			for (const FPrimaryAssetId& ID : AssetIDs)
			{
				if (UPrimaryDataAsset* Asset = Cast<UPrimaryDataAsset>(
					Mgr.GetPrimaryAssetObject(ID)))
				{
					LoadedAssets.Add(ID, Asset);
				}
			}
			OnAllLoaded.ExecuteIfBound();
		})
	);

	if (Handle.IsValid())
	{
		AssetsHandles.Add(Handle);
	}
}

void UAsyncDataManager::LoadAssetsByTypeAsync(FPrimaryAssetType AssetType, FOnAssetsLoaded OnAllLoaded)
{
	TArray<FPrimaryAssetId> IDs = GetAllAssetIDs(AssetType);

	if (IDs.Num() == 0)
	{
		OnAllLoaded.ExecuteIfBound();
		return;
	}

	LoadAssetsAsync(IDs, OnAllLoaded);
}

void UAsyncDataManager::UnloadAsset(FPrimaryAssetId AssetID)
{
	LoadedAssets.Remove(AssetID);

	if (TSharedPtr<FStreamableHandle>* Handle = LoadingHandles.Find(AssetID))
	{
		if (Handle->IsValid())
		{
			(*Handle)->CancelHandle();
		}
		LoadingHandles.Remove(AssetID);
	}

	UAssetManager::Get().UnloadPrimaryAsset(AssetID);
}

void UAsyncDataManager::UnloadAllByType(FPrimaryAssetType AssetType)
{
	TArray<FPrimaryAssetId> IDs = GetAllAssetIDs(AssetType);
	for (const FPrimaryAssetId& ID : IDs)
	{
		UnloadAsset(ID);
	}
}

UPrimaryDataAsset* UAsyncDataManager::GetLoadedAsset(FPrimaryAssetId AssetID) const
{
	if (const TObjectPtr<UPrimaryDataAsset>* Found = LoadedAssets.Find(AssetID))
	{
		return Found->Get();
	}
	return nullptr;
}

bool UAsyncDataManager::IsAssetLoaded(FPrimaryAssetId AssetID) const
{
	return LoadedAssets.Contains(AssetID);
}

bool UAsyncDataManager::IsAssetLoading(FPrimaryAssetId AssetID) const
{
	return LoadingHandles.Contains(AssetID);
}

TArray<FPrimaryAssetId> UAsyncDataManager::GetAllAssetIDs(FPrimaryAssetType AssetType) const
{
	TArray<FPrimaryAssetId> OutIDs;
	UAssetManager::Get().GetPrimaryAssetIdList(AssetType, OutIDs);
	return OutIDs;
}



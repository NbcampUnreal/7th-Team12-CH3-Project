// Fill out your copyright notice in the Description page of Project Settings.


#include"AsyncDataManager.h"
#include"Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

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

	bInitialLoadComplete = false;
	PendingInitialLoadCount = 2;
	LoadSingleBundle(FPrimaryAssetType("Item"), NAME_None, true);
	LoadSingleBundle(FPrimaryAssetType("Item"), FName("Lobby"), true);
}

void UAsyncDataManager::Deinitialize()
{
	for (auto& TypePair : BundleHandles)
	{
		for (auto& HandlePair : TypePair.Value)
		{
			if (HandlePair.Value.IsValid())
			{
				HandlePair.Value->CancelHandle();
			}
		}
	}
	BundleHandles.Empty();
	LoadedAssets.Empty();
	Super::Deinitialize();
}

void UAsyncDataManager::CheckInitialLoadComplete()
{
	if (PendingInitialLoadCount <= 0)
	{
		bInitialLoadComplete = true;
	}
}

bool UAsyncDataManager::IsInitialLoadComplete() const
{
	return bInitialLoadComplete;
}

void UAsyncDataManager::LoadBundles(FPrimaryAssetType AssetType, const TArray<FName>& Bundles)
{
	if (!BundleHandles.Contains(AssetType) || !BundleHandles[AssetType].Contains(NAME_None))
	{
		LoadSingleBundle(AssetType, NAME_None, false);
	}

	for (const FName& Bundle : Bundles)
	{
		if (TMap<FName, TSharedPtr<FStreamableHandle>>* TypeHandles = BundleHandles.Find(AssetType))
		{
			if (TypeHandles->Contains(Bundle))
			{
				continue;
			}
		}

		LoadSingleBundle(AssetType, Bundle, false);
	}
}

void UAsyncDataManager::UnloadBundles(FPrimaryAssetType AssetType, const TArray<FName>& Bundles)
{
	TMap<FName, TSharedPtr<FStreamableHandle>>* TypeHandles = BundleHandles.Find(AssetType);
	if (!TypeHandles) return;

	for (const FName& Bundle : Bundles)
	{
		if (Bundle == NAME_None) continue;

		if (TSharedPtr<FStreamableHandle>* Handle = TypeHandles->Find(Bundle))
		{
			if (Handle->IsValid())
			{
				(*Handle)->ReleaseHandle();
			}
			TypeHandles->Remove(Bundle);
		}
	}
}

void UAsyncDataManager::UnloadAllByType(FPrimaryAssetType AssetType)
{
	if (TMap<FName, TSharedPtr<FStreamableHandle>>* TypeHandles = BundleHandles.Find(AssetType))
	{
		for (auto& HandlePair : *TypeHandles)
		{
			if (HandlePair.Value.IsValid())
			{
				HandlePair.Value->ReleaseHandle();
			}
		}
		BundleHandles.Remove(AssetType);
	}

	TArray<FPrimaryAssetId> IDs = GetAllAssetIDs(AssetType);
	for (const FPrimaryAssetId& ID : IDs)
	{
		LoadedAssets.Remove(ID);
		UAssetManager::Get().UnloadPrimaryAsset(ID);
	}
}

void UAsyncDataManager::LoadSingleBundle(FPrimaryAssetType AssetType, FName BundleName, bool bTrackCompletion)
{
	TArray<FPrimaryAssetId> IDs = GetAllAssetIDs(AssetType);

	if (IDs.Num() == 0)
	{
		if (bTrackCompletion)
		{
			--PendingInitialLoadCount;
			CheckInitialLoadComplete();
		}
		return;
	}

	TArray<FName> BundleArray;
	if (BundleName != NAME_None)
	{
		BundleArray.Add(BundleName);
	}

	UAssetManager& Manager = UAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = Manager.LoadPrimaryAssets(
		IDs,
		BundleArray,
		FStreamableDelegate::CreateLambda([this, IDs, BundleName, bTrackCompletion]()
		{
			if (BundleName == NAME_None)
			{
				UAssetManager& Mgr = UAssetManager::Get();
				for (const FPrimaryAssetId& ID : IDs)
				{
					if (UPrimaryDataAsset* Asset = Cast<UPrimaryDataAsset>(
						Mgr.GetPrimaryAssetObject(ID)))
					{
						LoadedAssets.Add(ID, Asset);
					}
				}
			}

			if (bTrackCompletion)
			{
				--PendingInitialLoadCount;
				CheckInitialLoadComplete();
			}
		})
	);

	if (Handle.IsValid())
	{
		BundleHandles.FindOrAdd(AssetType).Add(BundleName, Handle);
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

TArray<FPrimaryAssetId> UAsyncDataManager::GetAllAssetIDs(FPrimaryAssetType AssetType) const
{
	TArray<FPrimaryAssetId> OutIDs;
	UAssetManager::Get().GetPrimaryAssetIdList(AssetType, OutIDs);
	return OutIDs;
}

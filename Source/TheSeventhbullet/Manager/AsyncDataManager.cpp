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

	LoadSingleBundle(FPrimaryAssetType("Item"), NAME_None, FOnBundleLoadComplete());
	LoadSingleBundle(FPrimaryAssetType("Item"), FName("UI"), FOnBundleLoadComplete());
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

void UAsyncDataManager::LoadBundles(FPrimaryAssetType AssetType, const TArray<FName>& Bundles, FOnBundleLoadComplete OnComplete)
{
	if (!BundleHandles.Contains(AssetType) || !BundleHandles[AssetType].Contains(NAME_None))
	{
		LoadSingleBundle(AssetType, NAME_None, FOnBundleLoadComplete());
	}

	if (Bundles.Num() == 0)
	{
		LoadSingleBundle(AssetType, NAME_None, OnComplete);
		return;
	}

	TArray<FName> BundlesToLoad;

	for (const FName& Bundle : Bundles)
	{
		if (TMap<FName, TSharedPtr<FStreamableHandle>>* TypeHandles = BundleHandles.Find(AssetType))
		{
			if (TypeHandles->Contains(Bundle))
			{
				continue;
			}
		}
		BundlesToLoad.Add(Bundle);
	}

	if (BundlesToLoad.Num() == 0)
	{
		if (OnComplete.IsBound())
		{
			OnComplete.Execute();
		}
		return;
	}

	TSharedPtr<int32> PendingCount = MakeShared<int32>(BundlesToLoad.Num());

	for (const FName& Bundle : BundlesToLoad)
	{
		FOnBundleLoadComplete BundleCallback;
		BundleCallback.BindLambda([PendingCount, OnComplete]()
		{
			--(*PendingCount);
			if (*PendingCount <= 0 && OnComplete.IsBound())
			{
				OnComplete.Execute();
			}
		});

		LoadSingleBundle(AssetType, Bundle, BundleCallback);
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

void UAsyncDataManager::LoadSingleBundle(FPrimaryAssetType AssetType, FName BundleName, FOnBundleLoadComplete OnComplete)
{
	TArray<FPrimaryAssetId> IDs = GetAllAssetIDs(AssetType);

	if (IDs.Num() == 0)
	{
		if (OnComplete.IsBound())
		{
			OnComplete.Execute();
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
		FStreamableDelegate::CreateLambda([this, IDs, BundleName, OnComplete]()
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

			if (OnComplete.IsBound())
			{
				OnComplete.Execute();
			}
		})
	);

	if (Handle.IsValid())
	{
		BundleHandles.FindOrAdd(AssetType).Add(BundleName, Handle);
	}
}

void UAsyncDataManager::LoadAssetsByID(const TArray<FPrimaryAssetId>& AssetIDs, const TArray<FName>& Bundles, FOnBundleLoadComplete OnComplete)
{
	TArray<FPrimaryAssetId> IDsToLoad;
	for (const FPrimaryAssetId& ID : AssetIDs)
	{
		if (ID.IsValid() && !LoadedAssets.Contains(ID))
		{
			IDsToLoad.Add(ID);
		}
	}

	if (IDsToLoad.Num() == 0)
	{
		if (OnComplete.IsBound())
		{
			OnComplete.Execute();
		}
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = Manager.LoadPrimaryAssets(
		IDsToLoad,
		Bundles,
		FStreamableDelegate::CreateLambda([this, IDsToLoad, OnComplete]()
		{
			UAssetManager& Mgr = UAssetManager::Get();
			for (const FPrimaryAssetId& ID : IDsToLoad)
			{
				if (UPrimaryDataAsset* Asset = Cast<UPrimaryDataAsset>(
					Mgr.GetPrimaryAssetObject(ID)))
				{
					LoadedAssets.Add(ID, Asset);
				}
			}

			if (OnComplete.IsBound())
			{
				OnComplete.Execute();
			}
		})
	);
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
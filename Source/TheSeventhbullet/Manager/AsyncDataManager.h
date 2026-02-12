// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"CoreMinimal.h"
#include"Subsystems/GameInstanceSubsystem.h"
#include"Engine/StreamableManager.h"
#include"AsyncDataManager.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [AsyncDataManager Guide]
 * 1. Load : LoadBundles로 타입의 DataAsset + 번들 리소스를 비동기 로드
 * 2. Load : UnloadBundles / UnloadAllByType으로 해제
 *
 * 번들 예시 :
 * 	LoadSingleBundle(FPrimaryAssetType("Item"), NAME_None, true);
 * 	LoadSingleBundle(FPrimaryAssetType("Item"), FName("Lobby"), true);
 * 	
 *  *** 사용 예시 : 
 *  UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
 *  if (Mgr->IsInitialLoadComplete()) // Initial Load로 Lobby 관련 데이터 및 메타데이터 로드
 *  {
 *      UItemDataAsset* Data = Cast<UItemDataAsset>(Mgr->GetLoadedAsset(ItemID));
 *      UTexture2D* Icon = Data->Icon.Get();  // Texture나 Image 같은 경우 Get을 통해 반환 가능
 *  }
 *
 * Load 및 Unload 예시 
 *  던전 입장: Mgr->LoadBundles(FPrimaryAssetType("Item"), {"InGame"});
 *  던전 퇴장: Mgr->UnloadBundles(FPrimaryAssetType("Item"), {"InGame"});
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class THESEVENTHBULLET_API UAsyncDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UAsyncDataManager* Get(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	void LoadBundles(FPrimaryAssetType AssetType, const TArray<FName>& Bundles);
	
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	void UnloadBundles(FPrimaryAssetType AssetType, const TArray<FName>& Bundles);

	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	void UnloadAllByType(FPrimaryAssetType AssetType);

	UFUNCTION(BlueprintPure, Category = "Data|Async")
	bool IsInitialLoadComplete() const;

	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	UPrimaryDataAsset* GetLoadedAsset(FPrimaryAssetId AssetID) const;

	UFUNCTION(BlueprintPure, Category = "Data|Async")
	bool IsAssetLoaded(FPrimaryAssetId AssetID) const;

	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	TArray<FPrimaryAssetId> GetAllAssetIDs(FPrimaryAssetType AssetType) const;

private:
	UPROPERTY()
	TMap<FPrimaryAssetId, TObjectPtr<UPrimaryDataAsset>> LoadedAssets;

	TMap<FPrimaryAssetType, TMap<FName, TSharedPtr<FStreamableHandle>>> BundleHandles;

	bool bInitialLoadComplete = false;
	int32 PendingInitialLoadCount = 0;

	void CheckInitialLoadComplete();
	
	void LoadSingleBundle(FPrimaryAssetType AssetType, FName BundleName, bool bTrackCompletion = false);
};

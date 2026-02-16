// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"CoreMinimal.h"
#include"Subsystems/GameInstanceSubsystem.h"
#include"Engine/StreamableManager.h"
#include"AsyncDataManager.generated.h"

DECLARE_DELEGATE(FOnBundleLoadComplete);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [AsyncDataManager Guide]
 * 1. Load : LoadBundles로 타입의 DataAsset + 번들 리소스를 비동기 로드
 * 2. Load : UnloadBundles / UnloadAllByType으로 해제
 *
 * 번들 예시 :
 * 	LoadBundles(FPrimaryAssetType("Item"), {}, OnComplete);          // 메타데이터만
 * 	LoadBundles(FPrimaryAssetType("Item"), {"Lobby"}, OnComplete);   // Lobby 번들
 *
 *  *** 사용 예시 :
 *  UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
 *  if (Mgr->IsAssetLoaded(ItemID))
 *  {
 *      UItemDataAsset* Data = Cast<UItemDataAsset>(Mgr->GetLoadedAsset(ItemID));
 *      UTexture2D* Icon = Data->Icon.Get();
 *  }
 *
 * Load 및 Unload 예시
 *  던전 입장: Mgr->LoadBundles(FPrimaryAssetType("Item"), {"InGame"}, OnComplete);
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

	void LoadBundles(FPrimaryAssetType AssetType, const TArray<FName>& Bundles, FOnBundleLoadComplete OnComplete);
	void UnloadBundles(FPrimaryAssetType AssetType, const TArray<FName>& Bundles);
	void UnloadAllByType(FPrimaryAssetType AssetType);
	
public:
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

	void LoadSingleBundle(FPrimaryAssetType AssetType, FName BundleName, FOnBundleLoadComplete OnComplete);
};

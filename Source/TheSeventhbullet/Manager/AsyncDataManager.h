// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"CoreMinimal.h"
#include"Subsystems/GameInstanceSubsystem.h"
#include"Engine/StreamableManager.h"
#include"AsyncDataManager.generated.h"

DECLARE_DELEGATE(FOnBundleLoadComplete);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [AsyncDataManager Guide]
 * 
 * 로드 방식 
 * 1. Initialize나 씬 전환 과정에서 대용량 로드 
 * -> 로딩화면으로 로드 확인 
 * -> 이후 GetLoadedAsset으로 캐싱 가져오기 
 * 
 * 2. 지역적으로 로드하는 경우 Delegate 사용
 * 
 * YS : 두가지 방식으로 제작해놨고 사용 방식은 전체적인 흐름을 확인해서 구현해야할거같다 
 *
 * [조회]
 *   - IsAssetLoaded(AssetID)    : 로드 여부 확인
 *   - GetLoadedAsset(AssetID)   : 로드된 DataAsset 포인터 반환 (없으면 nullptr)
 *   - GetAllAssetIDs(AssetType) : AssetManager에 등록된 해당 타입의 전체 ID 목록
 *
 * [Delegate]
 *  사용 예시 1. LoadBundles (번들에 따라 로드)
 *   UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
 *   FOnBundleLoadComplete OnComplete;
 *   OnComplete.BindLambda([this, Mgr]()
 *   {
 *       FPrimaryAssetId SwordID = FPrimaryAssetId("Item:Sword");
 *       UItemDataAsset* Data = Cast<UItemDataAsset>(Mgr->GetLoadedAsset(SwordID)); // Sword만 꺼내 쓰는 Case
 *       if (Data)
 *       {
 *           FText  Name  = Data->DisplayName;
 *           int32  Stack = Data->MaxStackCount;
 *           UTexture2D* Icon = Data->Icon.Get();  // 번들 로드 후 .Get()
 *       }
 *   });
 *   Mgr->LoadBundles(FPrimaryAssetType("Item"), {"InGame"}, OnComplete);
 *
 *  사용 예시 2. LoadAssetsByID (특정 ID만 로드)
 *   UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
 *   TArray<FPrimaryAssetId> IDs;
 *   IDs.Add(FPrimaryAssetId("Item:Sword"));
 *   IDs.Add(FPrimaryAssetId("Item:Shield"));
 *   IDs.Add(FPrimaryAssetId("Item:Potion"));
 *
 *   FOnBundleLoadComplete OnComplete;
 *   OnComplete.BindLambda([this, Mgr, IDs]()
 *   {
 *       TMap<FPrimaryAssetId, UItemDataAsset*> ItemMap; // 로드하고 싶은 ID들을 맵형태로 저장 Case
 *       for (const FPrimaryAssetId& ID : IDs)
 *       {
 *           if (UItemDataAsset* Data = Cast<UItemDataAsset>(Mgr->GetLoadedAsset(ID)))
 *           {
 *               ItemMap.Add(ID, Data);
 *           }
 *       }
 *   });
 *   Mgr->LoadAssetsByID(IDs, {}, OnComplete);
 *
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
	void LoadAssetsByID(const TArray<FPrimaryAssetId>& AssetIDs, const TArray<FName>& Bundles, FOnBundleLoadComplete OnComplete);

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

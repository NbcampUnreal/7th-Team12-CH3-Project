// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"CoreMinimal.h"
#include"Subsystems/GameInstanceSubsystem.h"
#include"Engine/StreamableManager.h"
#include"AsyncDataManager.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [AsynDataManager Guide]
 * Texture, Mesh 같은 액터들을 로드 할때는 AsyncLoad를 이용한다 
 * -> Primary Data Asset에 관련 액터정보 저장 - 이때 TSoftObjectPtr를 사용해 약한 참조를 이용
 * -> 관련 파일들을 가진 폴더를 AssetManager 경로 지정
 * -> 이후 DataManager를 통해 사용시 로드
 * 
 * 사용 방법
 *  1. Manager 캐싱
 * UAsyncDataManager* AsyncManager = GetOwner()->GetGameInstance()->GetSubsystem<UAsyncDataManager>();
 * 
 *  2 Load 되었는지 확인 
 *  if (AsyncMgr->IsAssetLoaded(ItemID)) 
 *  -> UPrimaryDataAsset* Data = AsyncManager->GetLoadedAsset(ItemID);
 *  -> Data를 이용해 Logic에 사용
 *  이때 ItemID는 Primary Data Asset을 구별하는 코드로 FPrimaryAssetId타입 (Enum 혹은 ItemTag같은 역할)
 *  
 *  3. Load X -> 비동기 로드 후 사용
 *  FOnAssetLoaded OnLoaded; // AsyncLoad에서 쓰는 델리게이트, 이 헤더에 선언되어있다
 *  OnLoaded.BindDynamic(this, &UInventoryComponent::OnEquipItemLoaded); //인벤토리에서 아이템 장착 델리게이트 예시
 *  AsyncManager->LoadAssetAsync(ItemID, OnLoaded);
 *  
 *  3-a. 로드된 아이템 사용
 *  void UInventoryComponent::OnEquipItemLoaded(UPrimaryDataAsset* LoadedAsset)
 *  {
 *	if (UItemDataAsset* ItemData = Cast<UItemDataAsset>(LoadedAsset))
 *		{
 *			ApplyEquipment(ItemData);
 *		}
 *  }
 *  4. 사용하지 않는다면 UnloadAsset
 *  -> 이거는 어느 시점에 해야 할 지 아직 감이 안옴
 *  -> 아마 레벨이나 마을 전투 전환시 안쓰는 것들 Unload하는걸 Stage 관리에서 사용해야할듯함
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Delegate Declaration
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAssetLoaded, UPrimaryDataAsset*, LoadedAsset);
DECLARE_DYNAMIC_DELEGATE(FOnAssetsLoaded);

UCLASS()
class THESEVENTHBULLET_API UAsyncDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	static UAsyncDataManager* Get(const UObject* WorldContextObject);
	
	//Load
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	void LoadAssetAsync(FPrimaryAssetId AssetID, FOnAssetLoaded OnLoaded);
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	void LoadAssetsAsync(TArray<FPrimaryAssetId> AssetIDs, FOnAssetsLoaded OnAllLoaded);
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	void LoadAssetsByTypeAsync(FPrimaryAssetType AssetType, FOnAssetsLoaded OnAllLoaded);

	//Unload
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	void UnloadAsset(FPrimaryAssetId AssetID);
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	void UnloadAllByType(FPrimaryAssetType AssetType);
	
	//Utility
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	UPrimaryDataAsset* GetLoadedAsset(FPrimaryAssetId AssetID) const;
	UFUNCTION(BlueprintPure, Category = "Data|Async")
	bool IsAssetLoaded(FPrimaryAssetId AssetID) const;
	UFUNCTION(BlueprintPure, Category = "Data|Async")
	bool IsAssetLoading(FPrimaryAssetId AssetID) const;
	UFUNCTION(BlueprintCallable, Category = "Data|Async")
	TArray<FPrimaryAssetId> GetAllAssetIDs(FPrimaryAssetType AssetType) const;
	
private:
	UPROPERTY()
	TMap<FPrimaryAssetId, TObjectPtr<UPrimaryDataAsset>> LoadedAssets;
	
	TMap<FPrimaryAssetId, TSharedPtr<FStreamableHandle>> LoadingHandles;
	
	TArray<TSharedPtr<FStreamableHandle>> AssetsHandles;
};

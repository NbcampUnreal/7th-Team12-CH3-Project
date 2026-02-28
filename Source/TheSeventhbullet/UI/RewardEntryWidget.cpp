#include "RewardEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/TableRowTypes.h"
#include "DataAsset/MaterialDataAsset.h"
#include "Manager/AsyncDataManager.h"

void URewardEntryWidget::SetData(const FDroppedMaterialsData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("[RewardEntry] SetData - Material=%s, Count=%d"),
		*Data.Material.GetAssetName(), Data.Count);

	// MaterialDataAsset 로드 (캐시 우선, 폴백 동기 로드)
	UMaterialDataAsset* MatAsset = nullptr;
	if (UAsyncDataManager* Mgr = UAsyncDataManager::Get(this))
	{
		FPrimaryAssetId AssetID = FPrimaryAssetId(
			FPrimaryAssetType("Item"),
			FName(*Data.Material.GetAssetName()));
		MatAsset = Cast<UMaterialDataAsset>(Mgr->GetLoadedAsset(AssetID));
		UE_LOG(LogTemp, Warning, TEXT("[RewardEntry] AsyncDataManager cache=%s"), MatAsset ? TEXT("HIT") : TEXT("MISS"));
	}
	if (!MatAsset)
	{
		MatAsset = Data.Material.LoadSynchronous();
		UE_LOG(LogTemp, Warning, TEXT("[RewardEntry] LoadSynchronous=%s"), MatAsset ? TEXT("OK") : TEXT("FAILED"));
	}
	if (!MatAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("[RewardEntry] MaterialDataAsset load FAILED - skipping"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[RewardEntry] DisplayName=%s"), *MatAsset->DisplayName.ToString());

	if (ItemIconImage)
	{
		UTexture2D* IconTexture = MatAsset->Icon.Get();
		if (!IconTexture)
		{
			IconTexture = MatAsset->Icon.LoadSynchronous();
		}
		if (IconTexture)
		{
			ItemIconImage->SetBrushFromTexture(IconTexture);
		}
	}

	if (ItemNameText)
	{
		ItemNameText->SetText(MatAsset->DisplayName);
	}

	if (ItemCountText)
	{
		ItemCountText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), Data.Count)));
	}
}
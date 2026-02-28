#include "ItemDropNotifyWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DataAsset/MaterialDataAsset.h"
#include "Manager/AsyncDataManager.h"
#include "Animation/WidgetAnimation.h"

void UItemDropNotifyWidget::SetDropInfo(const FDroppedMaterialsData& DropData)
{
	// AsyncDataManager 캐시에서 먼저 조회, 없으면 동기 로드 폴백
	UMaterialDataAsset* MatAsset = nullptr;
	if (UAsyncDataManager* Mgr = UAsyncDataManager::Get(this))
	{
		FPrimaryAssetId AssetID = FPrimaryAssetId(FPrimaryAssetType("Item"), FName(*DropData.Material.GetAssetName()));
		MatAsset = Cast<UMaterialDataAsset>(Mgr->GetLoadedAsset(AssetID));
	}
	if (!MatAsset)
	{
		MatAsset = DropData.Material.LoadSynchronous();
	}
	if (!MatAsset)
	{
		return;
	}

	if (ItemNameText)
	{
		FString Msg = FString::Printf(TEXT("'%s'을(를) 획득했습니다"), *MatAsset->DisplayName.ToString());
		ItemNameText->SetText(FText::FromString(Msg));
	}

	if (ItemCountText)
	{
		ItemCountText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), DropData.Count)));
	}

	if (ItemIconImage)
	{
		// "UI" 번들로 로드된 경우 Icon.Get()으로 즉시 획득 가능
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
}

void UItemDropNotifyWidget::PlayHide()
{
	if (HideAnim)
	{
		FWidgetAnimationDynamicEvent EndDelegate;
		EndDelegate.BindDynamic(this, &UItemDropNotifyWidget::OnHideAnimComplete);
		BindToAnimationFinished(HideAnim, EndDelegate);
		PlayAnimation(HideAnim);
	}
	else
	{
		// 애니메이션 없으면 즉시 완료 처리
		OnHideAnimComplete();
	}
}

void UItemDropNotifyWidget::OnHideAnimComplete()
{
	OnHideFinished.ExecuteIfBound();
}

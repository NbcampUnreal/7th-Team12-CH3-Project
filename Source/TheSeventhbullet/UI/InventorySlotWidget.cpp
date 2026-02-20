#include "InventorySlotWidget.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Manager/AsyncDataManager.h"
#include "DataAsset/ItemDataAsset.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SlotButton && !SlotButton->OnClicked.IsAlreadyBound(this, &UInventorySlotWidget::OnSlotClicked))
	{
		SlotButton->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnSlotClicked);
	}
}

void UInventorySlotWidget::UpdateSlot(const FItemInstance& Item)
{
	CachedItemID = Item.ItemID;

	if (!Item.IsValid())
	{
		ClearSlot();
		return;
	}

	if (Item.StackCount > 1)
	{
		CountText->SetText(FText::AsNumber(Item.StackCount));
		CountText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CountText->SetVisibility(ESlateVisibility::Collapsed);
	}

	UAsyncDataManager* DataMgr = UAsyncDataManager::Get(this);
	if (!DataMgr)
	{
		return;
	}

	UItemDataAsset* ItemData = Cast<UItemDataAsset>(DataMgr->GetLoadedAsset(Item.ItemID));
	if (!ItemData)
	{
		return;
	}

	UTexture2D* IconTexture = ItemData->Icon.Get();
	SetIcon(IconTexture);
}

void UInventorySlotWidget::ClearSlot()
{
	CachedItemID = FPrimaryAssetId();
	CountText->SetVisibility(ESlateVisibility::Collapsed);
	SetIcon(nullptr);
}

void UInventorySlotWidget::SetIcon(UTexture2D* Texture)
{
	if (Texture)
	{
		IconImage->SetBrushFromTexture(Texture);
		IconImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventorySlotWidget::OnSlotClicked()
{
	
}

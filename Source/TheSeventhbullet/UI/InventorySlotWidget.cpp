#include "InventorySlotWidget.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Manager/AsyncDataManager.h"
#include "DataAsset/ItemDataAsset.h"
#include "ItemTooltipWidget.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

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
	if (IconTexture)
	{
		SetIcon(IconTexture);
	}
	else if (!ItemData->Icon.IsNull())
	{
		TSoftObjectPtr<UTexture2D> IconPtr = ItemData->Icon;
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(
			IconPtr.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(this, [this, IconPtr]()
			{
				SetIcon(IconPtr.Get());
			})
		);
	}
	else
	{
		SetIcon(nullptr);
	}

	if (TooltipWidgetClass)
	{
		UItemTooltipWidget* TooltipWidget = CreateWidget<UItemTooltipWidget>(this, TooltipWidgetClass);
		if (TooltipWidget)
		{
			TooltipWidget->SetItemInfo(ItemData->DisplayName, ItemData->Description);
			SetToolTip(TooltipWidget);
		}
	}
}

void UInventorySlotWidget::ClearSlot()
{
	CachedItemID = FPrimaryAssetId();
	CountText->SetVisibility(ESlateVisibility::Collapsed);
	SetIcon(nullptr);
	SetToolTip(nullptr);
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

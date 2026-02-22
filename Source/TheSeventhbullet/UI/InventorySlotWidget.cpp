#include "InventorySlotWidget.h"
#include "InventoryDragDropOperation.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Manager/AsyncDataManager.h"
#include "DataAsset/ItemDataAsset.h"
#include "ItemTooltipWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventorySlotWidget::InitSlotInfo(UInventoryComponent* InInventory, int32 InSlotIndex)
{
	OwningInventory = InInventory;
	SlotIndex = InSlotIndex;
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && CachedItemID.IsValid())
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("[Drag] 3. DragDetected - Slot:%d, Inv:%d"),
		SlotIndex, OwningInventory != nullptr);

	if (!OwningInventory || SlotIndex == INDEX_NONE || !CachedItemID.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Drag] 3. BLOCKED - Inv:%d, Slot:%d, Item:%d"),
			OwningInventory != nullptr, SlotIndex, CachedItemID.IsValid());
		return;
	}

	UInventoryDragDropOperation* DragOp = NewObject<UInventoryDragDropOperation>();
	DragOp->SourceInventory = OwningInventory;
	DragOp->SourceSlotIndex = SlotIndex;
	DragOp->DraggedItem = OwningInventory->GetItemAt(SlotIndex);
	
	if (IconImage && IconImage->GetVisibility() != ESlateVisibility::Collapsed)
	{
		USizeBox* DragSizeBox = NewObject<USizeBox>(this);
		DragSizeBox->SetWidthOverride(80.f);
		DragSizeBox->SetHeightOverride(80.f);

		UImage* DragImage = NewObject<UImage>(DragSizeBox);
		DragImage->SetBrush(IconImage->GetBrush());
		DragImage->SetRenderOpacity(0.7f);
		DragSizeBox->AddChild(DragImage);

		DragOp->DefaultDragVisual = DragSizeBox;
		DragOp->Pivot = EDragPivot::CenterCenter;
		DragOp->Offset = FVector2D::ZeroVector;
	}

	OutOperation = DragOp;
}

bool UInventorySlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bAccept = Cast<UInventoryDragDropOperation>(InOperation) != nullptr;
	return bAccept;
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	
	UInventoryDragDropOperation* DragOp = Cast<UInventoryDragDropOperation>(InOperation);
	if (!DragOp || !DragOp->SourceInventory || !OwningInventory)
	{
		return false;
	}
	
	if (DragOp->SourceInventory == OwningInventory && DragOp->SourceSlotIndex == SlotIndex)
	{
		return false;
	}
	
	if (DragOp->SourceInventory == OwningInventory)
	{
		return OwningInventory->SwapSlots(DragOp->SourceSlotIndex, SlotIndex);
	}
	else
	{
		return DragOp->SourceInventory->MoveItemTo(DragOp->SourceSlotIndex, OwningInventory, SlotIndex);
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
			TooltipWidget->SetItemInfo(ItemData->DisplayName, ItemData->Description, Item.StackCount, ItemData->Icon.Get());
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
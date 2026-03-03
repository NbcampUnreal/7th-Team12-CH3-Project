#include "GemSocketSlotWidget.h"
#include "InventoryDragDropOperation.h"
#include "ItemTooltipWidget.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Manager/AsyncDataManager.h"
#include "DataAsset/SoulGemDataAsset.h"
#include "Character/Component/EquipmentComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UGemSocketSlotWidget::InitSlot(UEquipmentComponent* InEquipComp, UInventoryComponent* InPlayerInv, int32 InSlotIndex)
{
	EquipmentComp = InEquipComp;
	PlayerInventory = InPlayerInv;
	SlotIndex = InSlotIndex;

	// 기존 장착 상태 표시
	if (EquipmentComp)
	{
		FSoulGemInstance Gem = EquipmentComp->GetSoulGemAt(SlotIndex);
		if (Gem.IsValid())
		{
			CachedSoulGem = Gem;
			// ItemID는 알 수 없으므로 아이콘 없이 툴팁만 표시
			UpdateTooltip(Gem, FPrimaryAssetId());
		}
		else
		{
			ClearSlot();
		}
	}
	else
	{
		ClearSlot();
	}
}

bool UGemSocketSlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Cast<UInventoryDragDropOperation>(InOperation) != nullptr;
}

bool UGemSocketSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UInventoryDragDropOperation* DragOp = Cast<UInventoryDragDropOperation>(InOperation);
	if (!DragOp || !DragOp->SourceInventory || !EquipmentComp)
	{
		return false;
	}

	// 소울젬인지 체크
	const FItemInstance& DraggedItem = DragOp->DraggedItem;
	if (!DraggedItem.IsSoulGem())
	{
		return false;
	}

	// 소울젬 장착 (기존 소울젬이 있으면 덮어쓰기 — 기존 젬은 파괴)
	EquipmentComp->EquipSoulGem(DraggedItem.SoulGemData, SlotIndex);

	// 인벤토리에서 제거
	DragOp->SourceInventory->RemoveItemByIndex(DragOp->SourceSlotIndex, 1);

	// 캐싱
	CachedItemID = DraggedItem.ItemID;
	CachedSoulGem = DraggedItem.SoulGemData;

	// 아이콘 및 툴팁 업데이트
	UpdateSlot(CachedSoulGem, CachedItemID);

	return true;
}

void UGemSocketSlotWidget::UpdateSlot(const FSoulGemInstance& SoulGem, FPrimaryAssetId InItemID)
{
	CachedSoulGem = SoulGem;
	CachedItemID = InItemID;

	// 아이콘 로드
	LoadAndSetIcon(InItemID);

	// 툴팁 업데이트
	UpdateTooltip(SoulGem, InItemID);
}

void UGemSocketSlotWidget::ClearSlot()
{
	CachedSoulGem = FSoulGemInstance();
	CachedItemID = FPrimaryAssetId();
	SetIcon(nullptr);
	SetToolTip(nullptr);
}

void UGemSocketSlotWidget::UpdateTooltip(const FSoulGemInstance& SoulGem, FPrimaryAssetId ItemID)
{
	if (!TooltipWidgetClass || !SoulGem.IsValid())
	{
		SetToolTip(nullptr);
		return;
	}

	UItemTooltipWidget* TooltipWidget = CreateWidget<UItemTooltipWidget>(this, TooltipWidgetClass);
	if (!TooltipWidget)
	{
		return;
	}

	FText DisplayName = SoulGem.GemName;
	FText Description = FText::FromString(SoulGem.ToDescriptionString());
	UTexture2D* IconTexture = nullptr;

	if (ItemID.IsValid())
	{
		UAsyncDataManager* DataMgr = UAsyncDataManager::Get(this);
		if (DataMgr)
		{
			USoulGemDataAsset* GemDA = Cast<USoulGemDataAsset>(DataMgr->GetLoadedAsset(ItemID));
			if (GemDA)
			{
				IconTexture = GemDA->Icon.Get();
			}
		}
	}

	TooltipWidget->SetItemInfo(DisplayName, Description, 1, IconTexture);
	SetToolTip(TooltipWidget);
}

void UGemSocketSlotWidget::LoadAndSetIcon(FPrimaryAssetId ItemID)
{
	UAsyncDataManager* DataMgr = UAsyncDataManager::Get(this);
	if (!DataMgr) return;

	USoulGemDataAsset* GemDA = Cast<USoulGemDataAsset>(DataMgr->GetLoadedAsset(ItemID));
	if (!GemDA) return;

	UTexture2D* IconTexture = GemDA->Icon.Get();
	if (IconTexture)
	{
		SetIcon(IconTexture);
	}
	else if (!GemDA->Icon.IsNull())
	{
		TSoftObjectPtr<UTexture2D> IconPtr = GemDA->Icon;
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
}

void UGemSocketSlotWidget::SetIcon(UTexture2D* Texture)
{
	if (!IconImage) return;

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

#include "GemSocketSlotWidget.h"
#include "ConfirmDialogWidget.h"
#include "InventoryDragDropOperation.h"
#include "ItemTooltipWidget.h"
#include "UITags.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Manager/AsyncDataManager.h"
#include "Manager/UIManager.h"
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
	bPendingConfirm = false;

	// 기존 장착 상태 표시
	if (EquipmentComp)
	{
		FSoulGemInstance Gem = EquipmentComp->GetSoulGemAt(SlotIndex);
		if (Gem.IsValid())
		{
			UpdateSlot(Gem, Gem.ItemID);
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

	// 확인 모달 대기 중이면 추가 드롭 거부
	if (bPendingConfirm)
	{
		return false;
	}

	// 소울젬인지 체크
	const FItemInstance& DraggedItem = DragOp->DraggedItem;
	if (!DraggedItem.IsSoulGem())
	{
		return false;
	}

	// 빈 슬롯이면 즉시 장착
	if (!CachedSoulGem.IsValid())
	{
		ExecuteEquip(DraggedItem.SoulGemData, DraggedItem.ItemID,
			DragOp->SourceInventory, DragOp->SourceSlotIndex);
		return true;
	}

	// 이미 장착된 슬롯 → 확인 모달
	bPendingConfirm = true;

	// 드롭은 수락하되 인벤토리에서 아직 제거하지 않고 캐싱
	FSoulGemInstance PendingSoulGem = DraggedItem.SoulGemData;
	FPrimaryAssetId PendingItemID = DraggedItem.ItemID;
	UInventoryComponent* SourceInv = DragOp->SourceInventory;
	int32 SourceSlotIdx = DragOp->SourceSlotIndex;

	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr)
	{
		bPendingConfirm = false;
		return false;
	}

	UUserWidget* DialogWidget = UIMgr->Open(UITags::ConfirmDialog);
	UConfirmDialogWidget* ConfirmDialog = Cast<UConfirmDialogWidget>(DialogWidget);
	if (!ConfirmDialog)
	{
		bPendingConfirm = false;
		return false;
	}

	ConfirmDialog->ShowDialog(
		FText::FromString(TEXT("기존 소울젬은 파괴됩니다.\n교체하시겠습니까?")),
		FOnConfirmDialogResult::CreateWeakLambda(this,
			[this, PendingSoulGem, PendingItemID, SourceInv, SourceSlotIdx](bool bConfirmed)
			{
				bPendingConfirm = false;

				if (bConfirmed)
				{
					ExecuteEquip(PendingSoulGem, PendingItemID, SourceInv, SourceSlotIdx);
				}
			})
	);

	return true;
}

void UGemSocketSlotWidget::ExecuteEquip(const FSoulGemInstance& SoulGemData, FPrimaryAssetId ItemID,
	UInventoryComponent* SourceInv, int32 SourceSlotIndex)
{
	if (!EquipmentComp) return;

	// 소울젬 장착 (기존 젬 덮어쓰기 = 파괴)
	EquipmentComp->EquipSoulGem(SoulGemData, SlotIndex);

	// 인벤토리에서 제거
	if (SourceInv)
	{
		SourceInv->RemoveItemByIndex(SourceSlotIndex, 1);
	}

	// 캐싱
	CachedItemID = ItemID;
	CachedSoulGem = SoulGemData;

	// 아이콘 및 툴팁 업데이트
	UpdateSlot(CachedSoulGem, CachedItemID);
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

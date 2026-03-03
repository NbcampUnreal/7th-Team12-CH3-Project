#include "GemSocketSlotWidget.h"
#include "InventoryDragDropOperation.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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
			// 이미 장착된 소울젬이 있으면 표시 (ItemID는 알 수 없으므로 이름만 표시)
			CachedSoulGem = Gem;
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

	// 이미 소울젬이 장착되어 있으면 거부
	if (CachedSoulGem.IsValid())
	{
		return false;
	}

	// 소울젬인지 체크
	const FItemInstance& DraggedItem = DragOp->DraggedItem;
	if (!DraggedItem.IsSoulGem())
	{
		return false;
	}

	// 소울젬 장착
	EquipmentComp->EquipSoulGem(DraggedItem.SoulGemData, SlotIndex);

	// 인벤토리에서 제거
	DragOp->SourceInventory->RemoveItemByIndex(DragOp->SourceSlotIndex, 1);

	// 캐싱
	CachedItemID = DraggedItem.ItemID;
	CachedSoulGem = DraggedItem.SoulGemData;

	// 아이콘 및 라벨 업데이트
	UpdateSlot(CachedSoulGem, CachedItemID);

	return true;
}

FReply UGemSocketSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 우클릭으로 장착 해제
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && CachedSoulGem.IsValid())
	{
		if (EquipmentComp && PlayerInventory)
		{
			// 장착 해제
			EquipmentComp->UnequipSoulGem(SlotIndex);

			// 인벤토리에 소울젬 복귀
			PlayerInventory->AddSoulGem(CachedItemID, CachedSoulGem);

			// 슬롯 클리어
			ClearSlot();
		}

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UGemSocketSlotWidget::UpdateSlot(const FSoulGemInstance& SoulGem, FPrimaryAssetId InItemID)
{
	CachedSoulGem = SoulGem;
	CachedItemID = InItemID;

	// 아이콘 로드
	LoadAndSetIcon(InItemID);
}

void UGemSocketSlotWidget::ClearSlot()
{
	CachedSoulGem = FSoulGemInstance();
	CachedItemID = FPrimaryAssetId();
	SetIcon(nullptr);
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

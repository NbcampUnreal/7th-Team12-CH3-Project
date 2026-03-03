#include "CraftingSlotWidget.h"
#include "InventoryDragDropOperation.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Manager/AsyncDataManager.h"
#include "DataAsset/MaterialDataAsset.h"
#include "Inventory/InventoryComponent.h"
#include "Interaction/CraftingComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UCraftingSlotWidget::InitSlot(UCraftingComponent* InCraftingComp, UInventoryComponent* InPlayerInv, int32 InSlotIndex)
{
	CraftingComp = InCraftingComp;
	PlayerInventory = InPlayerInv;
	SlotIndex = InSlotIndex;
	ClearSlot();
}

bool UCraftingSlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Cast<UInventoryDragDropOperation>(InOperation) != nullptr;
}

bool UCraftingSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UInventoryDragDropOperation* DragOp = Cast<UInventoryDragDropOperation>(InOperation);
	if (!DragOp || !DragOp->SourceInventory || !CraftingComp)
	{
		return false;
	}

	// 이미 재료가 있으면 드롭 거부
	if (CachedSourceItem.IsValid())
	{
		return false;
	}

	// AsyncDataManager에서 ItemID로 MaterialDataAsset 로드/캐스팅
	UAsyncDataManager* DataMgr = UAsyncDataManager::Get(this);
	if (!DataMgr)
	{
		return false;
	}

	UMaterialDataAsset* MaterialData = Cast<UMaterialDataAsset>(DataMgr->GetLoadedAsset(DragOp->DraggedItem.ItemID));
	if (!MaterialData)
	{
		// 재료 아이템이 아니면 드롭 거부
		return false;
	}

	// CraftingComponent에 재료 추가
	if (!CraftingComp->AddMaterial(MaterialData))
	{
		return false;
	}

	// 인벤토리에서 재료 제거
	DragOp->SourceInventory->RemoveItemByIndex(DragOp->SourceSlotIndex, 1);

	// 반환용으로 원본 아이템 정보 캐싱
	CachedSourceItem = FItemInstance(DragOp->DraggedItem.ItemID, 1);

	// 아이콘 표시
	UpdateSlot(MaterialData);

	return true;
}

FReply UCraftingSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 우클릭으로 재료 반환
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && CachedSourceItem.IsValid())
	{
		if (CraftingComp && PlayerInventory)
		{
			// CraftingComponent에서 재료 제거
			CraftingComp->RemoveMaterial(SlotIndex);

			// 인벤토리에 재료 복귀
			PlayerInventory->AddItem(CachedSourceItem.ItemID, 1);

			// 슬롯 클리어
			ClearSlot();
		}

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UCraftingSlotWidget::UpdateSlot(UMaterialDataAsset* Material)
{
	if (!Material)
	{
		ClearSlot();
		return;
	}

	// 아이콘 로드
	UTexture2D* IconTexture = Material->Icon.Get();
	if (IconTexture)
	{
		SetIcon(IconTexture);
	}
	else if (!Material->Icon.IsNull())
	{
		TSoftObjectPtr<UTexture2D> IconPtr = Material->Icon;
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

	// 슬롯 라벨에 재료 이름 표시
	if (SlotLabel)
	{
		SlotLabel->SetText(Material->DisplayName);
	}
}

void UCraftingSlotWidget::ClearSlot()
{
	CachedSourceItem = FItemInstance();
	SetIcon(nullptr);

	if (SlotLabel)
	{
		SlotLabel->SetText(FText::Format(
			NSLOCTEXT("Crafting", "SlotLabel", "슬롯 {0}"),
			FText::AsNumber(SlotIndex + 1)
		));
	}
}

void UCraftingSlotWidget::SetIcon(UTexture2D* Texture)
{
	if (!IconImage)
	{
		return;
	}

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

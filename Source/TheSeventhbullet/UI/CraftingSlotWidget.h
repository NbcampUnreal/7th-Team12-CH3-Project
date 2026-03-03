////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * CraftingSlotWidget - 제작 UI의 재료 슬롯 위젯
 *
 * 인벤토리에서 드래그한 재료를 드롭하여 배치하는 슬롯.
 * 우클릭으로 재료를 인벤토리에 반환.
 *
 * [사용법]
 *   1. CraftingWidget에서 4개 슬롯을 BindWidget으로 배치
 *   2. InitSlot(CraftingComponent, SlotIndex)로 초기화
 *   3. 인벤토리 슬롯에서 드래그 → 이 슬롯에 드롭
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemInstance.h"
#include "CraftingSlotWidget.generated.h"

class USizeBox;
class UImage;
class UTextBlock;
class UCraftingComponent;
class UInventoryComponent;
class UMaterialDataAsset;

UCLASS()
class THESEVENTHBULLET_API UCraftingSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitSlot(UCraftingComponent* InCraftingComp, UInventoryComponent* InPlayerInv, int32 InSlotIndex);

	void UpdateSlot(UMaterialDataAsset* Material);
	void ClearSlot();

	FItemInstance GetCachedSourceItem() const { return CachedSourceItem; }
	bool HasMaterial() const { return CachedSourceItem.IsValid(); }

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SlotSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SlotLabel;

private:
	void SetIcon(UTexture2D* Texture);

	UPROPERTY()
	TObjectPtr<UCraftingComponent> CraftingComp;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> PlayerInventory;

	int32 SlotIndex = INDEX_NONE;

	FItemInstance CachedSourceItem;
};

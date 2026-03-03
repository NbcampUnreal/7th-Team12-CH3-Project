////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * GemSocketSlotWidget - 소울젬 장착 슬롯 위젯
 *
 * 인벤토리에서 소울젬을 드래그하여 드롭하면 장착.
 * 우클릭으로 장착 해제하여 인벤토리로 복귀.
 *
 * [사용법]
 *   1. GemSocketWidget에서 7개 슬롯을 BindWidget으로 배치
 *   2. InitSlot(EquipmentComp, PlayerInv, SlotIndex)로 초기화
 *   3. 소울젬 드래그 드롭 → 장착
 *   4. 우클릭 → 해제
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemInstance.h"
#include "GemSocketSlotWidget.generated.h"

class USizeBox;
class UImage;
class UTextBlock;
class UEquipmentComponent;
class UInventoryComponent;

UCLASS()
class THESEVENTHBULLET_API UGemSocketSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitSlot(UEquipmentComponent* InEquipComp, UInventoryComponent* InPlayerInv, int32 InSlotIndex);

	void UpdateSlot(const FSoulGemInstance& SoulGem, FPrimaryAssetId InItemID);
	void ClearSlot();

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SlotSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

private:
	void SetIcon(UTexture2D* Texture);
	void LoadAndSetIcon(FPrimaryAssetId ItemID);

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComp;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> PlayerInventory;

	int32 SlotIndex = INDEX_NONE;

	// 장착된 소울젬의 원본 ItemID (인벤토리 복귀용)
	FPrimaryAssetId CachedItemID;
	FSoulGemInstance CachedSoulGem;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * GemSocketSlotWidget - 소울젬 장착 슬롯 위젯
 *
 * 인벤토리에서 소울젬을 드래그하여 드롭하면 장착.
 * 이미 장착된 슬롯에 드롭하면 확인 모달 표시 후 교체 (기존 젬 파괴).
 * 장착만 가능하며 탈착은 불가.
 *
 * [사용법]
 *   1. GemSocketWidget에서 7개 슬롯을 BindWidget으로 배치
 *   2. InitSlot(EquipmentComp, PlayerInv, SlotIndex)로 초기화
 *   3. 소울젬 드래그 드롭 → 빈 슬롯: 즉시 장착 / 점유 슬롯: 확인 모달 → 장착
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemInstance.h"
#include "GemSocketSlotWidget.generated.h"

class USizeBox;
class UImage;
class UEquipmentComponent;
class UInventoryComponent;
class UItemTooltipWidget;

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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SlotSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Tooltip")
	TSubclassOf<UItemTooltipWidget> TooltipWidgetClass;

private:
	void SetIcon(UTexture2D* Texture);
	void LoadAndSetIcon(FPrimaryAssetId ItemID);
	void UpdateTooltip(const FSoulGemInstance& SoulGem, FPrimaryAssetId ItemID);

	// 실제 장착 수행
	void ExecuteEquip(const FSoulGemInstance& SoulGemData, FPrimaryAssetId ItemID,
		UInventoryComponent* SourceInv, int32 SourceSlotIndex);

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComp;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> PlayerInventory;

	int32 SlotIndex = INDEX_NONE;

	FPrimaryAssetId CachedItemID;
	FSoulGemInstance CachedSoulGem;

	// 확인 모달 대기 중 캐시
	bool bPendingConfirm = false;
};

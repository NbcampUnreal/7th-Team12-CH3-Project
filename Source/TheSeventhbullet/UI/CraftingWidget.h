////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * CraftingWidget - 소울젬 제작 UI
 *
 * CraftingActor와 상호작용 시 열리는 제작 UI.
 * 좌측에 플레이어 인벤토리, 우측에 4개 재료 슬롯 + 제작 버튼.
 * 인벤토리에서 재료를 드래그하여 슬롯에 배치 → 제작 버튼 → Modal 팝업으로 결과 표시.
 *
 * [레이아웃]
 *   ┌─ CraftingWidget ────────────────────────────────────┐
 *   │  ┌─ InventoryWidget ─┐   ┌─ Crafting Panel ──────┐ │
 *   │  │  (플레이어 인벤)    │   │  [Slot0] [Slot1]      │ │
 *   │  │  [slot][slot]...   │   │  [Slot2] [Slot3]      │ │
 *   │  │                    │   │  [ 제작 버튼 ]         │ │
 *   │  └────────────────────┘   └───────────────────────┘ │
 *   │                                       [닫기 버튼]   │
 *   └─────────────────────────────────────────────────────┘
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingWidget.generated.h"

class UInventoryWidget;
class UCraftingSlotWidget;
class UCraftingComponent;
class UInventoryComponent;
class UMaterialDataAsset;
class UButton;
enum class EMaterialTypes : uint8;

UCLASS()
class THESEVENTHBULLET_API UCraftingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Crafting")
	void OpenCrafting(UCraftingComponent* InCraftingComp, UInventoryComponent* InPlayerInv);

protected:
	virtual void NativeConstruct() override;

	// --- BindWidget ---

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> PlayerInventoryPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingSlotWidget> CraftingSlot_0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingSlotWidget> CraftingSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingSlotWidget> CraftingSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingSlotWidget> CraftingSlot_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CraftButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

private:
	UFUNCTION()
	void OnCraftClicked();

	UFUNCTION()
	void OnCloseClicked();

	void ReturnAllMaterials();
	FPrimaryAssetId DetermineSoulGemID(const TArray<UMaterialDataAsset*>& Materials) const;

	UPROPERTY()
	TObjectPtr<UCraftingComponent> CraftingComp;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> PlayerInventory;

	TArray<UCraftingSlotWidget*> CraftingSlots;
};

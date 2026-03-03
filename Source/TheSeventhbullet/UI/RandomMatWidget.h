////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * RandomMatWidget - 재료 무작위 변환 UI
 *
 * 재료 3개를 투입하면 완전 무작위 재료 1개를 생성.
 * 좌측에 플레이어 인벤토리, 우측에 3개 입력 슬롯 + 변환 버튼.
 *
 * [레이아웃]
 *   ┌─ RandomMatWidget ────────────────────────────────────┐
 *   │  ┌─ InventoryWidget ─┐   ┌─ RandomMat Panel ──────┐ │
 *   │  │  (플레이어 인벤)    │   │  [Slot0] [Slot1]       │ │
 *   │  │  [slot][slot]...   │   │  [Slot2]               │ │
 *   │  │                    │   │  [ 변환 버튼 ]          │ │
 *   │  └────────────────────┘   └────────────────────────┘ │
 *   │                                       [닫기 버튼]    │
 *   └──────────────────────────────────────────────────────┘
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RandomMatWidget.generated.h"

class UInventoryWidget;
class UCraftingSlotWidget;
class UCraftingComponent;
class UInventoryComponent;
class UButton;

UCLASS()
class THESEVENTHBULLET_API URandomMatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|RandomMat")
	void OpenRandomMat(UCraftingComponent* InCraftingComp, UInventoryComponent* InPlayerInv);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> PlayerInventoryPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingSlotWidget> RandomMatSlot_0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingSlotWidget> RandomMatSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingSlotWidget> RandomMatSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConvertButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

private:
	UFUNCTION()
	void OnConvertClicked();

	UFUNCTION()
	void OnCloseClicked();

	void ReturnAllMaterials();
	FPrimaryAssetId PickRandomMaterialID() const;

	UPROPERTY()
	TObjectPtr<UCraftingComponent> CraftingComp;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> PlayerInventory;

	TArray<UCraftingSlotWidget*> InputSlots;

	static constexpr int32 RequiredMaterialCount = 3;
};

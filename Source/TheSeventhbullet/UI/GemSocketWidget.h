////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * GemSocketWidget - 소울젬 장착 UI
 *
 * 소울젬을 인벤토리에서 드래그하여 7개 소켓에 배치하는 UI.
 * 좌측에 플레이어 인벤토리, 우측에 7개 소울젬 소켓.
 *
 * [레이아웃]
 *   ┌─ GemSocketWidget ───────────────────────────────────┐
 *   │  ┌─ InventoryWidget ─┐   ┌─ Socket Panel ────────┐ │
 *   │  │  (플레이어 인벤)    │   │  [Socket0] [Socket1]  │ │
 *   │  │  [slot][slot]...   │   │  [Socket2] [Socket3]  │ │
 *   │  │                    │   │  [Socket4] [Socket5]  │ │
 *   │  │                    │   │  [Socket6]            │ │
 *   │  └────────────────────┘   └───────────────────────┘ │
 *   │                                       [닫기 버튼]   │
 *   └─────────────────────────────────────────────────────┘
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GemSocketWidget.generated.h"

class UInventoryWidget;
class UGemSocketSlotWidget;
class UEquipmentComponent;
class UInventoryComponent;
class UButton;

UCLASS()
class THESEVENTHBULLET_API UGemSocketWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|GemSocket")
	void OpenGemSocket(UEquipmentComponent* InEquipComp, UInventoryComponent* InPlayerInv);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> PlayerInventoryPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGemSocketSlotWidget> GemSocketSlot_0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGemSocketSlotWidget> GemSocketSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGemSocketSlotWidget> GemSocketSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGemSocketSlotWidget> GemSocketSlot_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGemSocketSlotWidget> GemSocketSlot_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGemSocketSlotWidget> GemSocketSlot_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGemSocketSlotWidget> GemSocketSlot_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

private:
	UFUNCTION()
	void OnCloseClicked();

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComp;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> PlayerInventory;

	TArray<UGemSocketSlotWidget*> GemSocketSlots;
};

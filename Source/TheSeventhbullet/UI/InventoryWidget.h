#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemInstance.h"
#include "InventoryWidget.generated.h"

class UWrapBox;
class UInventoryComponent;
class UInventorySlotWidget;

UCLASS()
class THESEVENTHBULLET_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Inventory")
	void RefreshAllSlots();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> SlotWrapBox;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Inventory")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Inventory")
	int32 SlotCount = 30;

private:
	void InitializeSlots();

	UFUNCTION()
	void OnItemChanged(const FItemInstance& Item, int32 SlotIndex);

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComp;

	UPROPERTY()
	TArray<TObjectPtr<UInventorySlotWidget>> SlotWidgets;

	bool bSlotsInitialized = false;
};

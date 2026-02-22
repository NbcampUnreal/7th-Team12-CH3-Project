#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemInstance.h"
#include "InventorySlotWidget.generated.h"

class USizeBox;
class UImage;
class UTextBlock;
class UTexture2D;
class UItemTooltipWidget;
class UInventoryComponent;

UCLASS()
class THESEVENTHBULLET_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Inventory")
	void UpdateSlot(const FItemInstance& Item);

	UFUNCTION(BlueprintCallable, Category = "UI|Inventory")
	void ClearSlot();

	UFUNCTION(BlueprintPure, Category = "UI|Inventory")
	FPrimaryAssetId GetItemID() const { return CachedItemID; }
	
	void InitSlotInfo(UInventoryComponent* InInventory, int32 InSlotIndex);

	int32 GetSlotIndex() const { return SlotIndex; }
	UInventoryComponent* GetOwningInventory() const { return OwningInventory; }

protected:
	virtual void NativeConstruct() override;

	// Drag & Drop
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SlotSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountText;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Tooltip")
	TSubclassOf<UItemTooltipWidget> TooltipWidgetClass;

private:
	void SetIcon(UTexture2D* Texture);

	FPrimaryAssetId CachedItemID;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;

	int32 SlotIndex = INDEX_NONE;
};
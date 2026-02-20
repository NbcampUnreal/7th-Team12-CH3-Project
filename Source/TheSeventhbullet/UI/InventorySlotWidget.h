#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemInstance.h"
#include "InventorySlotWidget.generated.h"

class USizeBox;
class UButton;
class UImage;
class UTextBlock;
class UTexture2D;
class UItemTooltipWidget;

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

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SlotSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountText;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Tooltip")
	TSubclassOf<UItemTooltipWidget> TooltipWidgetClass;

private:
	UFUNCTION()
	void OnSlotClicked();

	void SetIcon(UTexture2D* Texture);

	FPrimaryAssetId CachedItemID;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * CraftingMenuWidget - 제작대 선택지 UI
 *
 * CraftingActor 상호작용 시 열리는 선택 메뉴.
 * "제작" / "장착" / "재료 변환" / "닫기" 4개 버튼.
 *
 * [레이아웃]
 *   ┌─ CraftingMenuWidget ──────┐
 *   │   [제작]                   │
 *   │   [장착]                   │
 *   │   [재료 변환]              │
 *   │   [닫기]                   │
 *   └───────────────────────────┘
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingMenuWidget.generated.h"

class UButton;
class UCraftingComponent;
class UInventoryComponent;
class UEquipmentComponent;

UCLASS()
class THESEVENTHBULLET_API UCraftingMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|CraftingMenu")
	void SetCraftingContext(UCraftingComponent* InCraftingComp, UInventoryComponent* InPlayerInv, UEquipmentComponent* InEquipComp);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CraftButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EquipButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RandomMatButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

private:
	UFUNCTION()
	void OnCraftClicked();

	UFUNCTION()
	void OnEquipClicked();

	UFUNCTION()
	void OnRandomMatClicked();

	UFUNCTION()
	void OnCloseClicked();

	UPROPERTY()
	TObjectPtr<UCraftingComponent> CraftingComp;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> PlayerInventory;

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComp;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * StorageWidget - 상자 인벤토리 + 플레이어 인벤토리를 나란히 표시하는 위젯
 *
 * ChestActor와 상호작용 시 UIManager를 통해 Push되며,
 * 두 InventoryWidget 간 드래그&드랍으로 아이템 이동 가능.
 *
 * [Blueprint 설정]
 *   WBP_StorageWidget에서 ChestInventoryPanel, PlayerInventoryPanel,
 *   CloseButton을 BindWidget으로 배치해야 함.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "WeaponSelectWidget.h"
#include "Blueprint/UserWidget.h"
#include "StorageWidget.generated.h"

class UInventoryWidget;
class UInventoryComponent;
class UButton;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UStorageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Storage")
	void OpenStorage(UInventoryComponent* ChestInv, UInventoryComponent* PlayerInv, AMainCharacter* InPlayer);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> ChestInventoryPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> PlayerInventoryPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWeaponSelectWidget> WeaponSelectPanel;
	
private:
	UFUNCTION()
	void OnCloseClicked();
};

#include "StorageWidget.h"
#include "InventoryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Manager/UIManager.h"

void UStorageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton && !CloseButton->OnClicked.IsAlreadyBound(this, &UStorageWidget::OnCloseClicked))
	{
		CloseButton->OnClicked.AddDynamic(this, &UStorageWidget::OnCloseClicked);
	}
}

void UStorageWidget::OpenStorage(UInventoryComponent* ChestInv, UInventoryComponent* PlayerInv)
{
	if (ChestInventoryPanel && ChestInv)
	{
		ChestInventoryPanel->SetInventoryComponent(ChestInv);
	}

	if (PlayerInventoryPanel && PlayerInv)
	{
		PlayerInventoryPanel->SetInventoryComponent(PlayerInv);
	}
}

void UStorageWidget::OnCloseClicked()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Pop();
	}
}

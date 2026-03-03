#include "StorageWidget.h"
#include "InventoryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Manager/UIManager.h"
#include "UITags.h"

void UStorageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton && !CloseButton->OnClicked.IsAlreadyBound(this, &UStorageWidget::OnCloseClicked))
	{
		CloseButton->OnClicked.AddDynamic(this, &UStorageWidget::OnCloseClicked);
	}
}

void UStorageWidget::OpenStorage(UInventoryComponent* ChestInv, UInventoryComponent* PlayerInv, AMainCharacter* InPlayer)
{
	if (ChestInventoryPanel && ChestInv)
	{
		ChestInventoryPanel->SetInventoryComponent(ChestInv);
	}

	if (PlayerInventoryPanel && PlayerInv)
	{
		PlayerInventoryPanel->SetInventoryComponent(PlayerInv);
	}
	if (WeaponSelectPanel && InPlayer)
	{
		WeaponSelectPanel->InitWeaponSelect(InPlayer);
	}
}

void UStorageWidget::OnCloseClicked()
{
	UE_LOG(LogTemp,Log,TEXT("Close Button Click"));
	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr) return;
	//
	// if (UIMgr->IsOpen(UITags::WeaponStorage))
	// {
	// 	UIMgr->Close(UITags::WeaponStorage);
	// }
    
	UIMgr->Close(UITags::Storage);
}

#include "GemSocketWidget.h"
#include "GemSocketSlotWidget.h"
#include "InventoryWidget.h"
#include "UITags.h"
#include "Components/Button.h"
#include "Character/Component/EquipmentComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Manager/UIManager.h"

void UGemSocketWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton && !CloseButton->OnClicked.IsAlreadyBound(this, &UGemSocketWidget::OnCloseClicked))
	{
		CloseButton->OnClicked.AddDynamic(this, &UGemSocketWidget::OnCloseClicked);
	}

	GemSocketSlots = {
		GemSocketSlot_0, GemSocketSlot_1, GemSocketSlot_2, GemSocketSlot_3,
		GemSocketSlot_4, GemSocketSlot_5, GemSocketSlot_6
	};
}

void UGemSocketWidget::OpenGemSocket(UEquipmentComponent* InEquipComp, UInventoryComponent* InPlayerInv)
{
	EquipmentComp = InEquipComp;
	PlayerInventory = InPlayerInv;

	// 인벤토리 패널 연결
	if (PlayerInventoryPanel && PlayerInventory)
	{
		PlayerInventoryPanel->SetInventoryComponent(PlayerInventory);
	}

	// 7개 소켓 슬롯 초기화
	for (int32 i = 0; i < GemSocketSlots.Num(); ++i)
	{
		if (GemSocketSlots[i])
		{
			GemSocketSlots[i]->InitSlot(EquipmentComp, PlayerInventory, i);
		}
	}
}

void UGemSocketWidget::OnCloseClicked()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::GemSocket);
	}
}

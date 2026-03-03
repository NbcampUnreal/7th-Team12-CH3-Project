#include "CraftingMenuWidget.h"
#include "CraftingWidget.h"
#include "GemSocketWidget.h"
#include "UITags.h"
#include "Components/Button.h"
#include "Manager/UIManager.h"

void UCraftingMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CraftButton && !CraftButton->OnClicked.IsAlreadyBound(this, &UCraftingMenuWidget::OnCraftClicked))
	{
		CraftButton->OnClicked.AddDynamic(this, &UCraftingMenuWidget::OnCraftClicked);
	}

	if (EquipButton && !EquipButton->OnClicked.IsAlreadyBound(this, &UCraftingMenuWidget::OnEquipClicked))
	{
		EquipButton->OnClicked.AddDynamic(this, &UCraftingMenuWidget::OnEquipClicked);
	}

	if (CloseButton && !CloseButton->OnClicked.IsAlreadyBound(this, &UCraftingMenuWidget::OnCloseClicked))
	{
		CloseButton->OnClicked.AddDynamic(this, &UCraftingMenuWidget::OnCloseClicked);
	}
}

void UCraftingMenuWidget::SetCraftingContext(UCraftingComponent* InCraftingComp, UInventoryComponent* InPlayerInv, UEquipmentComponent* InEquipComp)
{
	CraftingComp = InCraftingComp;
	PlayerInventory = InPlayerInv;
	EquipmentComp = InEquipComp;
}

void UCraftingMenuWidget::OnCraftClicked()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr) return;

	UIMgr->Close(UITags::CraftingMenu);

	UUserWidget* Widget = UIMgr->Open(UITags::Crafting);
	UCraftingWidget* CraftingWidget = Cast<UCraftingWidget>(Widget);
	if (CraftingWidget)
	{
		CraftingWidget->OpenCrafting(CraftingComp, PlayerInventory);
	}
}

void UCraftingMenuWidget::OnEquipClicked()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr) return;

	UIMgr->Close(UITags::CraftingMenu);

	UUserWidget* Widget = UIMgr->Open(UITags::GemSocket);
	UGemSocketWidget* GemSocketWidget = Cast<UGemSocketWidget>(Widget);
	if (GemSocketWidget)
	{
		GemSocketWidget->OpenGemSocket(EquipmentComp, PlayerInventory);
	}
}

void UCraftingMenuWidget::OnCloseClicked()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::CraftingMenu);
	}
}

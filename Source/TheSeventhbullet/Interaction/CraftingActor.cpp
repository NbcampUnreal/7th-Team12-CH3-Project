#include "CraftingActor.h"
#include "CraftingComponent.h"
#include "Character/MainCharacter.h"
#include "Character/Component/EquipmentComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Manager/UIManager.h"
#include "UI/UITags.h"
#include "UI/CraftingMenuWidget.h"

ACraftingActor::ACraftingActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CraftingComp = CreateDefaultSubobject<UCraftingComponent>(TEXT("CraftingComponent"));

	PromptText = FText::FromString(TEXT("F키를 눌러 제작대 사용"));
}

void ACraftingActor::Interact(AActor* Interactor)
{
	AMainCharacter* Player = Cast<AMainCharacter>(Interactor);
	if (!Player)
	{
		return;
	}

	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr)
	{
		return;
	}

	UUserWidget* Widget = UIMgr->Open(UITags::CraftingMenu);
	UCraftingMenuWidget* MenuWidget = Cast<UCraftingMenuWidget>(Widget);
	if (MenuWidget)
	{
		MenuWidget->SetCraftingContext(CraftingComp, Player->InventoryComponent, Player->EquipmentComponent);
	}
}

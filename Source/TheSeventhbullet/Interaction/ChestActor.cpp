#include "ChestActor.h"
#include "Inventory/InventoryComponent.h"
#include "Character/MainCharacter.h"
#include "Manager/UIManager.h"
#include "UI/StorageWidget.h"
#include "UI/UITags.h"

AChestActor::AChestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// 부모의 StaticMesh를 창고 메시로 활용
	// PromptText를 창고용으로 변경
	PromptText = FText::FromString(TEXT("F키를 눌러 상자 열기"));
}

void AChestActor::Interact(AActor* Interactor)
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

	UUserWidget* Widget = UIMgr->Open(UITags::Storage);
	UStorageWidget* StorageWidget = Cast<UStorageWidget>(Widget);
	if (StorageWidget)
	{
		StorageWidget->OpenStorage(InventoryComp, Player->InventoryComponent);
	}
}

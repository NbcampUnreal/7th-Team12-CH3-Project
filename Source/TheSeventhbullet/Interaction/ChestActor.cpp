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
	PromptText = FText::FromString(TEXT("F 상자 열기"));
}

void AChestActor::Interact(AActor* Interactor)
{
	AMainCharacter* Player = Cast<AMainCharacter>(Interactor);
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChestActor::Interact - Player cast failed"));
		return;
	}

	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChestActor::Interact - UIManager is null"));
		return;
	}

	UUserWidget* Widget = UIMgr->PushByTag(UITags::Storage);
	UE_LOG(LogTemp, Log, TEXT("ChestActor::Interact - PushByTag result: %s"), Widget ? *Widget->GetName() : TEXT("NULL"));

	UStorageWidget* StorageWidget = Cast<UStorageWidget>(Widget);
	if (StorageWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("ChestActor::Interact - OpenStorage called (ChestInv: %s, PlayerInv: %s)"),
			InventoryComp ? TEXT("Valid") : TEXT("NULL"),
			Player->InventoryComponent ? TEXT("Valid") : TEXT("NULL"));
		StorageWidget->OpenStorage(InventoryComp, Player->InventoryComponent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ChestActor::Interact - StorageWidget cast failed"));
	}
}

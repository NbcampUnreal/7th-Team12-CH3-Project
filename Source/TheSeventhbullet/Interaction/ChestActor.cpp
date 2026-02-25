#include "ChestActor.h"
#include "Inventory/InventoryComponent.h"

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
	// 카메라 전환 불필요 → Super::Interact 호출하지 않음
	// TODO: StorageWidget Push
	UE_LOG(LogTemp, Log, TEXT("ChestActor::Interact called by %s"), *Interactor->GetName());
}

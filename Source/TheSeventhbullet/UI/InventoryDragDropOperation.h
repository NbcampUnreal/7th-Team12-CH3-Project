#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Inventory/ItemInstance.h"
#include "InventoryDragDropOperation.generated.h"

class UInventoryComponent;

UCLASS()
class THESEVENTHBULLET_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> SourceInventory;

	UPROPERTY()
	int32 SourceSlotIndex = INDEX_NONE;

	UPROPERTY()
	FItemInstance DraggedItem;
};
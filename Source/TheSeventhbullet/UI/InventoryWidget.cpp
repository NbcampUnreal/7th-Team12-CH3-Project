#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "Components/WrapBox.h"
#include "Inventory/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!InventoryComp)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC && PC->GetPawn())
		{
			InventoryComp = PC->GetPawn()->FindComponentByClass<UInventoryComponent>();
		}
	}

	InitializeSlots();

	if (InventoryComp)
	{
		if (!InventoryComp->OnItemAdded.IsAlreadyBound(this, &UInventoryWidget::OnItemChanged))
		{
			InventoryComp->OnItemAdded.AddDynamic(this, &UInventoryWidget::OnItemChanged);
		}
		if (!InventoryComp->OnItemRemoved.IsAlreadyBound(this, &UInventoryWidget::OnItemChanged))
		{
			InventoryComp->OnItemRemoved.AddDynamic(this, &UInventoryWidget::OnItemChanged);
		}
		RefreshAllSlots();
	}
}

void UInventoryWidget::InitializeSlots()
{
	if (bSlotsInitialized || !SlotWrapBox || !SlotWidgetClass)
	{
		return;
	}

	SlotWrapBox->ClearChildren();
	SlotWidgets.Empty();

	for (int32 i = 0; i < SlotCount; ++i)
	{
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
		if (SlotWidget)
		{
			SlotWidget->InitSlotInfo(InventoryComp, i);
			SlotWrapBox->AddChild(SlotWidget);
			SlotWidgets.Add(SlotWidget);
		}
	}

	bSlotsInitialized = true;
}

void UInventoryWidget::RefreshAllSlots()
{
	if (!InventoryComp)
	{
		return;
	}

	TArray<FItemInstance> Items = InventoryComp->GetAllItems();

	for (int32 i = 0; i < SlotWidgets.Num(); ++i)
	{
		if (!SlotWidgets[i])
		{
			continue;
		}

		if (i < Items.Num() && Items[i].IsValid())
		{
			SlotWidgets[i]->UpdateSlot(Items[i]);
		}
		else
		{
			SlotWidgets[i]->ClearSlot();
		}
	}
}

void UInventoryWidget::OnItemChanged(const FItemInstance& Item, int32 SlotIndex)
{
	if (SlotWidgets.IsValidIndex(SlotIndex))
	{
		TArray<FItemInstance> Items = InventoryComp->GetAllItems();
		if (Items.IsValidIndex(SlotIndex) && Items[SlotIndex].IsValid())
		{
			SlotWidgets[SlotIndex]->UpdateSlot(Items[SlotIndex]);
		}
		else
		{
			SlotWidgets[SlotIndex]->ClearSlot();
		}
	}
	else
	{
		RefreshAllSlots();
	}
}

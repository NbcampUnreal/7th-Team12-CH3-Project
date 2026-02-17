// Fill out your copyright notice in the Description page of Project Settings.
#include "InventoryComponent.h"
#include "DataAsset/ItemDataAsset.h"
#include "Manager/AsyncDataManager.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UInventoryComponent::AddItem(FPrimaryAssetId ItemID, int32 Count)
{
	UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
	if (!Mgr || !Mgr->IsAssetLoaded(ItemID)) return false;

	UItemDataAsset* ItemData = Cast<UItemDataAsset>(Mgr->GetLoadedAsset(ItemID));
	if (!ItemData)
	{
		return false;
	}
	int32 Remaining = Count;
	int32 MaxStack = ItemData->MaxStackCount;
	
	if (MaxStack > 1)
	{
		int32 StackSlot = FindStackableSlot(ItemID, MaxStack);
		while (StackSlot != INDEX_NONE && Remaining > 0)
		{
			int32 CanAdd = FMath::Min(Remaining, MaxStack - Items[StackSlot].StackCount);
			Items[StackSlot].StackCount += CanAdd;
			Remaining -= CanAdd;

			OnItemAdded.Broadcast(Items[StackSlot], StackSlot);
			StackSlot = FindStackableSlot(ItemID, MaxStack);
		}
	}
	
	while (Remaining > 0 && Items.Num() < MaxSlots)
	{
		int32 AddCount = FMath::Min(Remaining, MaxStack);
		FItemInstance NewItem(ItemID, AddCount);
		int32 NewSlot = Items.Add(NewItem);
		Remaining -= AddCount;

		OnItemAdded.Broadcast(NewItem, NewSlot);
	}

	return Remaining == 0;
}

bool UInventoryComponent::RemoveItemByIndex(int32 SlotIndex, int32 Count)
{
	if (!Items.IsValidIndex(SlotIndex) || Count <= 0) return false;

	FItemInstance& Slot = Items[SlotIndex];
	if (!Slot.IsValid()) return false;

	int32 Removed = FMath::Min(Count, Slot.StackCount);
	Slot.StackCount -= Removed;

	FItemInstance RemovedItem(Slot.ItemID, Removed);

	if (Slot.StackCount <= 0)
	{
		Items.RemoveAt(SlotIndex);
	}

	OnItemRemoved.Broadcast(RemovedItem, SlotIndex);
	return true;
}

bool UInventoryComponent::RemoveItemByID(FPrimaryAssetId ItemID, int32 Count)
{
	if (!ItemID.IsValid() || Count <= 0) return false;

	int32 Remaining = Count;

	for (int32 i = Items.Num() - 1; i >= 0 && Remaining > 0; --i)
	{
		if (Items[i].ItemID != ItemID) continue;

		int32 Removed = FMath::Min(Remaining, Items[i].StackCount);
		Items[i].StackCount -= Removed;
		Remaining -= Removed;

		FItemInstance RemovedItem(ItemID, Removed);

		if (Items[i].StackCount <= 0)
		{
			Items.RemoveAt(i);
		}

		OnItemRemoved.Broadcast(RemovedItem, i);
	}

	return Remaining == 0;
}

FItemInstance UInventoryComponent::FindItemByID(FPrimaryAssetId ItemID) const
{
	for (const FItemInstance& Item : Items)
	{
		if (Item.ItemID == ItemID)
		{
			return Item;
		}
	}
	return FItemInstance();
}

int32 UInventoryComponent::FindStackableSlot(FPrimaryAssetId ItemID, int32 MaxStack) const
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].ItemID == ItemID && Items[i].StackCount < MaxStack)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

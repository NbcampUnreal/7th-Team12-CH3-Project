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
	if (!Mgr)
	{
		return false;
	}
	
	if (!Mgr->IsAssetLoaded(ItemID))
	{
		TArray<FPrimaryAssetId> IDs;
		IDs.Add(ItemID);

		FOnBundleLoadComplete OnLoaded;
		OnLoaded.BindLambda([this, ItemID, Count]()
		{
			AddItemInternal(ItemID, Count);
		});

		Mgr->LoadAssetsByID(IDs, {}, OnLoaded);
		return false; 
	}

	return AddItemInternal(ItemID, Count);
}

bool UInventoryComponent::AddItemInternal(FPrimaryAssetId ItemID, int32 Count)
{
	UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
	if (!Mgr)
	{
		return false;
	}

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

bool UInventoryComponent::SwapSlots(int32 FromIndex, int32 ToIndex)
{
	if (FromIndex == ToIndex) return false;
	if (FromIndex < 0 || FromIndex >= MaxSlots || ToIndex < 0 || ToIndex >= MaxSlots) return false;

	// 배열을 ToIndex까지 빈 슬롯으로 확장
	while (Items.Num() <= FMath::Max(FromIndex, ToIndex))
	{
		Items.Add(FItemInstance());
	}

	Items.Swap(FromIndex, ToIndex);

	OnItemAdded.Broadcast(Items[FromIndex], FromIndex);
	OnItemAdded.Broadcast(Items[ToIndex], ToIndex);
	return true;
}

bool UInventoryComponent::MoveItemTo(int32 FromIndex, UInventoryComponent* TargetInventory, int32 ToIndex)
{
	if (!TargetInventory || !Items.IsValidIndex(FromIndex)) return false;

	// 같은 인벤토리면 SwapSlots로 처리
	if (TargetInventory == this)
	{
		return SwapSlots(FromIndex, ToIndex);
	}

	FItemInstance SourceItem = Items[FromIndex];
	if (!SourceItem.IsValid()) return false;

	// 대상 슬롯에 아이템이 있으면 교환
	if (TargetInventory->Items.IsValidIndex(ToIndex) && TargetInventory->Items[ToIndex].IsValid())
	{
		FItemInstance TargetItem = TargetInventory->Items[ToIndex];

		Items[FromIndex] = TargetItem;
		TargetInventory->Items[ToIndex] = SourceItem;

		OnItemAdded.Broadcast(Items[FromIndex], FromIndex);
		TargetInventory->OnItemAdded.Broadcast(TargetInventory->Items[ToIndex], ToIndex);
	}
	else
	{
		// 대상 슬롯이 비어있으면 이동
		while (TargetInventory->Items.Num() <= ToIndex && TargetInventory->Items.Num() < TargetInventory->MaxSlots)
		{
			TargetInventory->Items.Add(FItemInstance());
		}

		if (!TargetInventory->Items.IsValidIndex(ToIndex)) return false;

		TargetInventory->Items[ToIndex] = SourceItem;
		Items.RemoveAt(FromIndex);

		OnItemRemoved.Broadcast(SourceItem, FromIndex);
		TargetInventory->OnItemAdded.Broadcast(SourceItem, ToIndex);
	}

	return true;
}

FItemInstance UInventoryComponent::GetItemAt(int32 SlotIndex) const
{
	if (Items.IsValidIndex(SlotIndex))
	{
		return Items[SlotIndex];
	}
	return FItemInstance();
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

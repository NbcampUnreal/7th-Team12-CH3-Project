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
	Items.SetNum(MaxSlots);
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

void UInventoryComponent::LoadData(TArray<FItemInstance>& InventoryItem)
{
	Items = InventoryItem;
	
	UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
	if (!Mgr) return;
	
	TArray<FPrimaryAssetId> IDsToLoad;
	for (const FItemInstance& Item : Items)
	{
		if (Item.IsValid() && !Mgr->IsAssetLoaded(Item.ItemID))
		{
			IDsToLoad.AddUnique(Item.ItemID);
		}
	}
	
	if (IDsToLoad.Num() > 0)
	{
		FOnBundleLoadComplete OnLoaded;
		OnLoaded.BindLambda([this]()
		{
			for (int32 i =0; i < Items.Num();++i)
			{
				OnItemAdded.Broadcast(Items[i],i);
			}
		});
		Mgr->LoadAssetsByID(IDsToLoad,{},OnLoaded);
	}
	else
	{
		for (int32 i = 0; i < Items.Num(); ++i)
		{
			OnItemAdded.Broadcast(Items[i], i);
		}
	}
	
}

void UInventoryComponent::ClearAllItems()
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].IsValid())
		{
			FItemInstance RemovedItem = Items[i];
			Items[i] = FItemInstance();
			OnItemRemoved.Broadcast(RemovedItem, i);
		}
	}
	Items.Empty();
	Items.SetNum(MaxSlots);
}

bool UInventoryComponent::AddItemInternal(FPrimaryAssetId ItemID, int32 Count)
{
	UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
	if (!Mgr)
	{
		return false;
	}

	UObject* LoadedObj = Mgr->GetLoadedAsset(ItemID);
	if (!LoadedObj)
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory] 에셋이 로드되지 않았습니다! ID: %s"), *ItemID.ToString());
		return false;
	}

	UItemDataAsset* ItemData = Cast<UItemDataAsset>(LoadedObj);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory] UItemDataAsset으로 캐스팅 실패! ID: %s, 실제 클래스: %s"), 
			*ItemID.ToString(), *LoadedObj->GetClass()->GetName());
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
	
	for (int32 i = 0; i < Items.Num()&& Remaining > 0 ; ++i)
	{
		if (!Items[i].IsValid())
		{
			int32 AddCount = FMath::Min(Remaining, MaxStack);
			Items[i] = FItemInstance(ItemID,AddCount);
			Remaining -= AddCount;
			
			OnItemAdded.Broadcast(Items[i],i);
		}
	}
	
	return Remaining == 0;
}

bool UInventoryComponent::AddSoulGem(FPrimaryAssetId ItemID, const FSoulGemInstance& SoulGemData)
{
	UE_LOG(LogTemp, Warning, TEXT("[AddSoulGem] 호출됨 - ItemID: %s, GemName: %s"), *ItemID.ToString(), *SoulGemData.GemName.ToString());

	UAsyncDataManager* Mgr = UAsyncDataManager::Get(this);
	if (!Mgr)
	{
		UE_LOG(LogTemp, Error, TEXT("[AddSoulGem] AsyncDataManager가 null"));
		return false;
	}

	auto PlaceGem = [this, ItemID, SoulGemData]()
	{
		for (int32 i = 0; i < Items.Num(); ++i)
		{
			if (!Items[i].IsValid())
			{
				Items[i].ItemID = ItemID;
				Items[i].StackCount = 1;
				Items[i].SoulGemData = SoulGemData;
				UE_LOG(LogTemp, Warning, TEXT("[AddSoulGem] PlaceGem 성공 - 슬롯: %d"), i);
				OnItemAdded.Broadcast(Items[i], i);
				return;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("[AddSoulGem] PlaceGem 실패: 빈 슬롯 없음"));
	};

	if (!Mgr->IsAssetLoaded(ItemID))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AddSoulGem] DA 미로드 → 비동기 로드 시작: %s"), *ItemID.ToString());
		TArray<FPrimaryAssetId> IDs;
		IDs.Add(ItemID);

		FOnBundleLoadComplete OnLoaded;
		OnLoaded.BindLambda([this, ItemID, PlaceGem]()
		{
			UE_LOG(LogTemp, Warning, TEXT("[AddSoulGem] 비동기 로드 완료 콜백 - ItemID: %s"), *ItemID.ToString());
			PlaceGem();
		});
		Mgr->LoadAssetsByID(IDs, {}, OnLoaded);
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("[AddSoulGem] DA 이미 로드됨 → 즉시 배치"));
	PlaceGem();
	return true;
}

bool UInventoryComponent::RemoveItemByIndex(int32 SlotIndex, int32 Count)
{
	if (!Items.IsValidIndex(SlotIndex) || Count <= 0) return false;

	FItemInstance& Slot = Items[SlotIndex];
	if (!Slot.IsValid()) return false;

	int32 Removed = FMath::Min(Count, Slot.StackCount);
	Slot.StackCount -= Removed;

	FItemInstance RemovedItem = Slot;
	RemovedItem.StackCount = Removed;

	if (Slot.StackCount <= 0)
	{
		Items[SlotIndex] = FItemInstance();
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

		FItemInstance RemovedItem = Items[i];
		RemovedItem.StackCount = Removed;

		if (Items[i].StackCount <= 0)
		{
			Items[i] = FItemInstance();
		}

		OnItemRemoved.Broadcast(RemovedItem, i);
	}

	return Remaining == 0;
}

bool UInventoryComponent::SwapSlots(int32 FromIndex, int32 ToIndex)
{
	if (FromIndex == ToIndex) return false;
	if (FromIndex < 0 || FromIndex >= MaxSlots || ToIndex < 0 || ToIndex >= MaxSlots) return false;

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
		if (!TargetInventory->Items.IsValidIndex(ToIndex)) return false;

		TargetInventory->Items[ToIndex] = SourceItem;
		Items[FromIndex] = FItemInstance();
		
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [Inventory Component]
 * 
 *  사용 예시 : Chracter -> InventoryCoponent -> AddItem( ItemID , 수량)
 *  Item ID Guide : 
 *  1.
 *  FPrimaryAssetId PotionID(FPrimaryAssetType("Item"), FName("DA_HealthPotion"));
 *  Inventory->AddItem(PotionID, 3);
 *  
 *  2.  
 *  FPrimaryAssetId ItemID;
 *  ItemID.FromString("Weapon:DA_AK47");  // "Type:Name" 형식
 *  Inventory->AddItem(ItemID, 1);
 *  
 *  DELEGATE로 UI 확장 준비중
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, const FItemInstance&, Item, int32, SlotIndex);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();
	virtual void BeginPlay() override;
	
	// Main Logic
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FPrimaryAssetId ItemID, int32 Count = 1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemByIndex(int32 SlotIndex, int32 Count = 1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemByID(FPrimaryAssetId ItemID, int32 Count = 1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemInstance FindItemByID(FPrimaryAssetId ItemID) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FItemInstance> GetAllItems() const { return Items; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasEmptySlot() const { return Items.Num() < MaxSlots; }

public: 
	//Delegate Instance
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnItemAdded;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnItemRemoved;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemInstance> Items;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 MaxSlots = 30;

private:
	bool AddItemInternal(FPrimaryAssetId ItemID, int32 Count);
	int32 FindStackableSlot(FPrimaryAssetId ItemID, int32 MaxStack) const;
};




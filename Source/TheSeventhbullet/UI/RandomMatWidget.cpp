#include "RandomMatWidget.h"
#include "CraftingSlotWidget.h"
#include "InventoryWidget.h"
#include "UITags.h"
#include "Components/Button.h"
#include "Interaction/CraftingComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Manager/AsyncDataManager.h"
#include "Manager/UIManager.h"
#include "DataAsset/MaterialDataAsset.h"

void URandomMatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConvertButton && !ConvertButton->OnClicked.IsAlreadyBound(this, &URandomMatWidget::OnConvertClicked))
	{
		ConvertButton->OnClicked.AddDynamic(this, &URandomMatWidget::OnConvertClicked);
	}

	if (CloseButton && !CloseButton->OnClicked.IsAlreadyBound(this, &URandomMatWidget::OnCloseClicked))
	{
		CloseButton->OnClicked.AddDynamic(this, &URandomMatWidget::OnCloseClicked);
	}

	InputSlots = { RandomMatSlot_0, RandomMatSlot_1, RandomMatSlot_2 };
}

void URandomMatWidget::OpenRandomMat(UCraftingComponent* InCraftingComp, UInventoryComponent* InPlayerInv)
{
	CraftingComp = InCraftingComp;
	PlayerInventory = InPlayerInv;

	if (PlayerInventoryPanel && PlayerInventory)
	{
		PlayerInventoryPanel->SetInventoryComponent(PlayerInventory);
	}

	for (int32 i = 0; i < InputSlots.Num(); ++i)
	{
		if (InputSlots[i])
		{
			InputSlots[i]->InitSlot(CraftingComp, PlayerInventory, i);
		}
	}
}

void URandomMatWidget::OnConvertClicked()
{
	if (!CraftingComp || !PlayerInventory)
	{
		return;
	}

	// 재료 3개 확인
	const TArray<UMaterialDataAsset*>& Materials = CraftingComp->GetSourceMaterials();
	if (Materials.Num() < RequiredMaterialCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RandomMat] 재료 부족 - 현재: %d, 필요: %d"), Materials.Num(), RequiredMaterialCount);
		return;
	}

	// 무작위 재료 ID 결정
	FPrimaryAssetId RandomID = PickRandomMaterialID();
	if (!RandomID.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RandomMat] 무작위 재료 선택 실패"));
		return;
	}

	// 재료 소모 (ClearMaterials)
	CraftingComp->ClearMaterials();

	// 슬롯 UI 클리어
	for (UCraftingSlotWidget* SlotIndex : InputSlots)
	{
		if (SlotIndex)
		{
			SlotIndex->ClearSlot();
		}
	}

	// 결과 재료를 인벤토리에 추가
	PlayerInventory->AddItem(RandomID, 1);

	UE_LOG(LogTemp, Warning, TEXT("[RandomMat] 변환 완료 → %s"), *RandomID.ToString());
}

void URandomMatWidget::OnCloseClicked()
{
	ReturnAllMaterials();

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::RandomMat);
	}
}

void URandomMatWidget::ReturnAllMaterials()
{
	if (!PlayerInventory || !CraftingComp)
	{
		return;
	}

	for (UCraftingSlotWidget* SlotEntry : InputSlots)
	{
		if (SlotEntry && SlotEntry->HasMaterial())
		{
			FItemInstance CachedItem = SlotEntry->GetCachedSourceItem();
			PlayerInventory->AddItem(CachedItem.ItemID, 1);
			SlotEntry->ClearSlot();
		}
	}

	CraftingComp->ClearMaterials();
}

FPrimaryAssetId URandomMatWidget::PickRandomMaterialID() const
{
	UAsyncDataManager* DataMgr = UAsyncDataManager::Get(this);
	if (!DataMgr)
	{
		return FPrimaryAssetId();
	}

	// "Item" 타입의 전체 AssetID 중 MaterialDataAsset만 필터링
	TArray<FPrimaryAssetId> AllItemIDs = DataMgr->GetAllAssetIDs(FPrimaryAssetType("Item"));
	TArray<FPrimaryAssetId> MaterialIDs;

	for (const FPrimaryAssetId& ID : AllItemIDs)
	{
		UPrimaryDataAsset* Asset = DataMgr->GetLoadedAsset(ID);
		if (Asset && Cast<UMaterialDataAsset>(Asset))
		{
			MaterialIDs.Add(ID);
		}
	}

	if (MaterialIDs.Num() == 0)
	{
		return FPrimaryAssetId();
	}

	int32 RandomIndex = FMath::RandRange(0, MaterialIDs.Num() - 1);
	return MaterialIDs[RandomIndex];
}

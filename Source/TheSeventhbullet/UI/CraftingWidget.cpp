#include "CraftingWidget.h"
#include "CraftingSlotWidget.h"
#include "CraftResultWidget.h"
#include "InventoryWidget.h"
#include "UITags.h"
#include "Components/Button.h"
#include "Interaction/CraftingComponent.h"
#include "Inventory/InventoryComponent.h"
#include "DataAsset/MaterialDataAsset.h"
#include "Manager/UIManager.h"

void UCraftingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CraftButton && !CraftButton->OnClicked.IsAlreadyBound(this, &UCraftingWidget::OnCraftClicked))
	{
		CraftButton->OnClicked.AddDynamic(this, &UCraftingWidget::OnCraftClicked);
	}

	if (CloseButton && !CloseButton->OnClicked.IsAlreadyBound(this, &UCraftingWidget::OnCloseClicked))
	{
		CloseButton->OnClicked.AddDynamic(this, &UCraftingWidget::OnCloseClicked);
	}

	// 슬롯 배열 구성
	CraftingSlots = { CraftingSlot_0, CraftingSlot_1, CraftingSlot_2, CraftingSlot_3 };
}

void UCraftingWidget::OpenCrafting(UCraftingComponent* InCraftingComp, UInventoryComponent* InPlayerInv)
{
	CraftingComp = InCraftingComp;
	PlayerInventory = InPlayerInv;

	// 인벤토리 패널 연결
	if (PlayerInventoryPanel && PlayerInventory)
	{
		PlayerInventoryPanel->SetInventoryComponent(PlayerInventory);
	}

	// 4개 CraftingSlot 초기화
	for (int32 i = 0; i < CraftingSlots.Num(); ++i)
	{
		if (CraftingSlots[i])
		{
			CraftingSlots[i]->InitSlot(CraftingComp, PlayerInventory, i);
		}
	}
}

void UCraftingWidget::OnCraftClicked()
{
	if (!CraftingComp || !PlayerInventory)
	{
		return;
	}

	// CraftSoulGem 내부에서 ClearMaterials() 호출하므로 제작 전에 재료 복사
	TArray<UMaterialDataAsset*> MaterialsCopy = CraftingComp->GetSourceMaterials();
	UE_LOG(LogTemp, Warning, TEXT("[CraftingWidget] OnCraftClicked - 재료 수: %d"), MaterialsCopy.Num());

	FSoulGemInstance Result;
	if (!CraftingComp->CraftSoulGem(Result))
	{
		UE_LOG(LogTemp, Warning, TEXT("[CraftingWidget] CraftSoulGem 실패 - 재료 부족"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[CraftingWidget] CraftSoulGem 성공 - GemName: %s, Grade: %d"), *Result.GemName.ToString(), Result.Grade);

	// 재료 타입별로 소울젬 DA 결정
	FPrimaryAssetId GemID = DetermineSoulGemID(MaterialsCopy);
	UE_LOG(LogTemp, Warning, TEXT("[CraftingWidget] DetermineSoulGemID → %s"), *GemID.ToString());

	PlayerInventory->AddSoulGem(GemID, Result);

	// 모든 슬롯 클리어 (CraftSoulGem 내부에서 ClearMaterials 호출됨)
	for (UCraftingSlotWidget* SlotEntry : CraftingSlots)
	{
		if (SlotEntry)
		{
			SlotEntry->ClearSlot();
		}
	}

	// Modal 팝업으로 결과 표시
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UUserWidget* ResultWidget = UIMgr->Open(UITags::CraftResult);
		UCraftResultWidget* CraftResult = Cast<UCraftResultWidget>(ResultWidget);
		if (CraftResult)
		{
			CraftResult->SetResult(Result, GemID);
		}
	}
}

void UCraftingWidget::OnCloseClicked()
{
	ReturnAllMaterials();

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::Crafting);
	}
}

void UCraftingWidget::ReturnAllMaterials()
{
	if (!PlayerInventory || !CraftingComp)
	{
		return;
	}

	for (UCraftingSlotWidget* SlotEntry : CraftingSlots)
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

FPrimaryAssetId UCraftingWidget::DetermineSoulGemID(const TArray<UMaterialDataAsset*>& Materials) const
{
	int32 CountA = 0, CountB = 0, CountC = 0;
	bool bHasSpecial = false;

	for (const UMaterialDataAsset* Mat : Materials)
	{
		if (!Mat) continue;

		switch (Mat->MaterialType)
		{
		case EMaterialTypes::A_Attack:        ++CountA; break;
		case EMaterialTypes::B_Utility:       ++CountB; break;
		case EMaterialTypes::C_Amplification: ++CountC; break;
		case EMaterialTypes::D_Special:       bHasSpecial = true; break;
		}
	}

	// D_Special 존재 → SoulGem_D
	if (bHasSpecial)
	{
		return FPrimaryAssetId(FPrimaryAssetType("SoulGem"), FName("DA_SoulGem_D"));
	}

	// 최다 타입 결정
	if (CountA > CountB && CountA > CountC)
	{
		return FPrimaryAssetId(FPrimaryAssetType("SoulGem"), FName("DA_SoulGem_A"));
	}
	if (CountB > CountA && CountB > CountC)
	{
		return FPrimaryAssetId(FPrimaryAssetType("SoulGem"), FName("DA_SoulGem_B"));
	}
	if (CountC > CountA && CountC > CountB)
	{
		return FPrimaryAssetId(FPrimaryAssetType("SoulGem"), FName("DA_SoulGem_C"));
	}

	// 균등 또는 동점
	return FPrimaryAssetId(FPrimaryAssetType("SoulGem"), FName("DA_SoulGem_Normal"));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSelectWidget.h"

#include "Character/Component/EquipmentComponent.h"
#include "Components/TextBlock.h"
#include "DataAsset/WeaponDataAsset.h"

void UWeaponSelectWidget::InitWeaponSelect(AMainCharacter* InCharacter)
{
	AllSlots = {PistolSlot, RifleSlot, ShotGunSlot};
	for (UWeaponSlotWidget* WeaponSlot : AllSlots)
	{
		if (WeaponSlot)
		{
			WeaponSlot->InitSlot(InCharacter);
			WeaponSlot->OnSlotSelected.AddUObject(this, &UWeaponSelectWidget::OnSlotSelected);
		}
	}

	// 이미 장착된 무기가 있으면 해당 슬롯 강조
	if (InCharacter)
	{
		if (UEquipmentComponent* EquipComp = InCharacter->FindComponentByClass<UEquipmentComponent>())
		{
			UWeaponDataAsset* Weapon = EquipComp->CurrentWeapon;
			if (!Weapon)
			{
				Weapon = EquipComp->PendingWeapon;
			}
			if (!Weapon)
			{
				Weapon = EquipComp->GetDefaultWeapon();
			}
			if (Weapon)
			{
				FPrimaryAssetId EquippedID = Weapon->GetPrimaryAssetId();
				for (UWeaponSlotWidget* SlotEntry : AllSlots)
				{
					if (SlotEntry)
					{
						SlotEntry->SetSelected(SlotEntry->GetWeaponAssetId() == EquippedID);
					}
				}
			}
		}
	}
}

void UWeaponSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("Weapon Select")));
	}
}

void UWeaponSelectWidget::OnSlotSelected(UWeaponSlotWidget* SelectedSlot)
{
	for (UWeaponSlotWidget* SlotEntry : AllSlots)
	{
		if (SlotEntry)
		{
			SlotEntry->SetSelected(SlotEntry == SelectedSlot);
		}
	}
}

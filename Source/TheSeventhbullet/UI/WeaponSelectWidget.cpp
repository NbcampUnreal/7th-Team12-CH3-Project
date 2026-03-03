// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSelectWidget.h"

#include "Components/TextBlock.h"

void UWeaponSelectWidget::InitWeaponSelect(AMainCharacter* InCharacter)
{
	TArray<UWeaponSlotWidget*> Slots = {PistolSlot, RifleSlot,ShotGunSlot};
	for (UWeaponSlotWidget* WeaponSlot : Slots)
	{
		if (WeaponSlot)
		{
			WeaponSlot->InitSlot(InCharacter);
		}
	}
}

void UWeaponSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("총기 선택")));
	}
}

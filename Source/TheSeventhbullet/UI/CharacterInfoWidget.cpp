#include "CharacterInfoWidget.h"
#include "Components/TextBlock.h"
#include "Character/MainCharacter.h"
#include "Character/Component/EquipmentComponent.h"
#include "DataAsset/WeaponDataAsset.h"
#include "System/GameInstance/MainGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UCharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshInfo();
}


void UCharacterInfoWidget::RefreshInfo()
{
	AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Character)
	{
		return;
	}

	const FCharacterStat& Stat = Character->GetTotalStatus();
	
	if (HPText)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("체력 : %d"), Stat.HP)));
	}
	if (StaminaText)
	{
		StaminaText->SetText(FText::FromString(FString::Printf(TEXT("스태미너 : %d"), Stat.Stamina)));
	}
	if (AttackText)
	{
		AttackText->SetText(FText::FromString(FString::Printf(TEXT("공격력 : %d"), Stat.Attack)));
	}
	if (DefenceText)
	{
		DefenceText->SetText(FText::FromString(FString::Printf(TEXT("방어력 : %d"), Stat.Defence)));
	}
	if (CritChanceText)
	{
		CritChanceText->SetText(FText::FromString(FString::Printf(TEXT("치명타 확률 : %.1f%%"), Stat.CriticalChance * 100.f)));
	}
	if (CritDamageText)
	{
		CritDamageText->SetText(FText::FromString(FString::Printf(TEXT("치명타 피해량 : %.1f%%"), Stat.CriticalDamage * 100.f)));
	}
	if (SpeedText)
	{
		SpeedText->SetText(FText::FromString(FString::Printf(TEXT("스피드 : %d"), Stat.Speed)));
	}
	if (GoldText)
	{
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("Gold :%d"), Character->GetGold())));
	}
	if (DayText)
	{
		if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance()))
		{
			DayText->SetText(FText::FromString(FString::Printf(TEXT("Day : %d"), GI->CurrentDay)));
		}
	}
	if (WeaponText)
	{
		FString WeaponStr = TEXT("무기 : 리볼버");
		if (UEquipmentComponent* EquipComp = Character->FindComponentByClass<UEquipmentComponent>())
		{
			UWeaponDataAsset* Weapon = EquipComp->CurrentWeapon;
			if (!Weapon)
			{
				Weapon = EquipComp->PendingWeapon;
			}
			if (Weapon)
			{
				switch (Weapon->WeaponType)
				{
				case EWeaponTypes::HandGun:       WeaponStr = TEXT("무기 : 리볼버"); break;
				case EWeaponTypes::AssaultRifle:   WeaponStr = TEXT("무기 : 라이플"); break;
				case EWeaponTypes::ShotGun:        WeaponStr = TEXT("무기 : 샷건"); break;
				default:                           WeaponStr = TEXT("무기 : 알 수 없음"); break;
				}
			}
		}
		WeaponText->SetText(FText::FromString(WeaponStr));
	}
}
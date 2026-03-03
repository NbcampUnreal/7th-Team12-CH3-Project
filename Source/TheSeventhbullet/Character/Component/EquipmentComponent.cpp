#include "EquipmentComponent.h"
#include "Character/MainCharacter.h"
#include "DataAsset/WeaponDataAsset.h"

void UEquipmentComponent::EquipSoulGem(const FSoulGemInstance& SoulGem, int32 SlotIndex)
{
	if (SlotIndex < 0) return;
	
	if (!EquippedSoulGems.IsValidIndex(SlotIndex))
	{
		EquippedSoulGems.SetNum(SlotIndex + 1);
	}
	
	EquippedSoulGems[SlotIndex] = SoulGem;
	OnGemEquipmentChanged.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("OnGemEquipmentChanged : Gem"));
}

void UEquipmentComponent::UnequipSoulGem(int32 SlotIndex)
{
	if (!EquippedSoulGems.IsValidIndex(SlotIndex)) return;

	EquippedSoulGems[SlotIndex] = FSoulGemInstance();
	OnGemEquipmentChanged.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("OnGemEquipmentChanged : UnequipGem SlotIndex=%d"), SlotIndex);
}

FSoulGemInstance UEquipmentComponent::GetSoulGemAt(int32 SlotIndex) const
{
	if (EquippedSoulGems.IsValidIndex(SlotIndex))
	{
		return EquippedSoulGems[SlotIndex];
	}
	return FSoulGemInstance();
}

void UEquipmentComponent::CollectStatusModifiers(TArray<FStatusModifier>& Mod) const
{
	for (const FSoulGemInstance& Gem : EquippedSoulGems)
	{
		Mod.Append(Gem.StatusModifiers);
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::CollectStatusModifiers"));
	}
}


bool UEquipmentComponent::FindSpecialOption(ESpecialOptions Option)
{
	for (const FSoulGemInstance& Gem : EquippedSoulGems)
	{
		for (const ESpecialOptions SpecialOption : Gem.SpecialOptions )
		{
			if (SpecialOption == Option)
			{
				return true;
			}
		}
	}
	return false;
}

FCharacterStat UEquipmentComponent::GetTotalGemStats() const
{
	FCharacterStat TotalGemStat;
	for (const FSoulGemInstance& Gem : EquippedSoulGems)
	{
		for (const FStatusModifier MaterialStat : Gem.StatusModifiers)
		{
			switch (MaterialStat.Status)
			{
			case EStatusType::ATK:
				if (MaterialStat.CalculationMethod == EStatusCalculationMethod::AddFlat)
				{
					TotalGemStat.Attack += MaterialStat.Value;
				}
				else if (MaterialStat.CalculationMethod == EStatusCalculationMethod::Multiply)
				{
					TotalGemStat.Attack *= MaterialStat.Value;
				}
				break;
			case EStatusType::DEF:
				if (MaterialStat.CalculationMethod == EStatusCalculationMethod::AddFlat)
				{
					TotalGemStat.Defence += MaterialStat.Value;
				}
				else if (MaterialStat.CalculationMethod == EStatusCalculationMethod::Multiply)
				{
					TotalGemStat.Defence *= MaterialStat.Value;
				}
				break;
			case EStatusType::HP:
				if (MaterialStat.CalculationMethod == EStatusCalculationMethod::AddFlat)
				{
					TotalGemStat.HP += MaterialStat.Value;
				}
				else if (MaterialStat.CalculationMethod == EStatusCalculationMethod::Multiply)
				{
					TotalGemStat.HP *= MaterialStat.Value;
				}
				break;
			case EStatusType::MaxSpeed:
				if (MaterialStat.CalculationMethod == EStatusCalculationMethod::AddFlat)
				{
					TotalGemStat.Speed += MaterialStat.Value;
				}
				else if (MaterialStat.CalculationMethod == EStatusCalculationMethod::Multiply)
				{
					TotalGemStat.Speed *= MaterialStat.Value;
				}
				break;
			case EStatusType::Stamina:
				if (MaterialStat.CalculationMethod == EStatusCalculationMethod::AddFlat)
				{
					TotalGemStat.Stamina += MaterialStat.Value;
				}
				else if (MaterialStat.CalculationMethod == EStatusCalculationMethod::Multiply)
				{
					TotalGemStat.Stamina *= MaterialStat.Value;
				}
				break;
			case EStatusType::CritChance:
				TotalGemStat.CriticalChance += MaterialStat.Value;
				TotalGemStat.CriticalChance = FMath::Clamp(TotalGemStat.CriticalChance, 0.0f, 1.0f);
				break;
			case EStatusType::CritDamage:
				TotalGemStat.CriticalDamage += MaterialStat.Value;
				TotalGemStat.CriticalDamage = FMath::Max(1.5f, TotalGemStat.CriticalDamage);
				break;
			default:
				break;
			}
		}
	}
	return TotalGemStat;
}

void UEquipmentComponent::SetPendingWeapon(UWeaponDataAsset* Weapon)
{
	PendingWeapon = Weapon;
	UE_LOG(LogTemp, Log, TEXT("무기 장착 예약 완료"));
}

void UEquipmentComponent::ApplyWeapon()
{
	if (!PendingWeapon)
	{
		if (DefaultWeapon)
		{
			PendingWeapon = DefaultWeapon;
			UE_LOG(LogTemp, Log, TEXT("PendingWeapon이 없어 DefaultWeapon으로 대체합니다."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("장착할 무기가 전혀 없습니다! (DefaultWeapon도 Null)"));
			return; 
		}
	}
	
	AMainCharacter* WeaponOwner = Cast<AMainCharacter>(GetOwner());
	if (!WeaponOwner || !WeaponOwner->WeaponMeshComponent) return;
	
	CurrentWeapon = PendingWeapon;
	WeaponOwner->WeaponMeshComponent->SetStaticMesh(CurrentWeapon->Mesh);
	OnWeaponEquipmentChanged.Broadcast(CurrentWeapon);
	PendingWeapon = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("OnWeaponEquipmentChanged : Weapon"));
}

void UEquipmentComponent::LoadData(TArray<FSoulGemInstance>& LoadEquippedSoulGems)
{
	EquippedSoulGems = LoadEquippedSoulGems;
}


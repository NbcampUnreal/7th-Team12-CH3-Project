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

void UEquipmentComponent::CollectStatusModifiers(TArray<FStatusModifier>& Mod) const
{
	for (const FSoulGemInstance& Gem : EquippedSoulGems)
	{
		Mod.Append(Gem.StatusModifiers);
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::CollectStatusModifiers"));
	}
}

void UEquipmentComponent::EquipWeaponData(UWeaponDataAsset* NewWeapon)
{
	AMainCharacter* WeaponOwner = Cast<AMainCharacter>(GetOwner());
	if (!WeaponOwner || !WeaponOwner->WeaponMeshComponent || !NewWeapon) return;
	
	CurrentWeapon = NewWeapon;
	WeaponOwner->WeaponMeshComponent->SetStaticMesh(NewWeapon->Mesh);
	OnWeaponEquipmentChanged.Broadcast(CurrentWeapon);
	UE_LOG(LogTemp, Warning, TEXT("OnWeaponEquipmentChanged : Weapon"));
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
				TotalGemStat.Attack += MaterialStat.Value;
				break;
			case EStatusType::DEF:
				TotalGemStat.Defence += MaterialStat.Value;
				break;
			case EStatusType::HP:
				TotalGemStat.HP += MaterialStat.Value;
				break;
			case EStatusType::MaxSpeed:
				TotalGemStat.Speed += MaterialStat.Value;
				break;
			case EStatusType::Stamina:
				TotalGemStat.Stamina += MaterialStat.Value;
				break;
			case EStatusType::CritChance:
				TotalGemStat.CriticalChance += MaterialStat.Value;
				TotalGemStat.CriticalChance = FMath::Clamp(TotalGemStat.CriticalChance, 0.0f, 0.85f);
				break;
			case EStatusType::CritDamage:
				TotalGemStat.CriticalDamage += MaterialStat.Value;
				TotalGemStat.CriticalDamage = FMath::Max(1.0f, TotalGemStat.CriticalDamage);
				break;
			default:
				break;
			}
		}
	}
	return TotalGemStat;
}


// void UGemStatusComponent::CalculateStatusFromModifiers(const TArray<FStatusModifier>& Modifiers)
// {
// 	FinalStatus = BaseStatus;
// 	
// 	for (const FStatusModifier& Mod : Modifiers)
// 	{
// 		if (Mod.CalculationMethod == EStatusCalculationMethod::AddFlat)
// 		{
// 			GetStatusRef(FinalStatus, Mod.Status) += Mod.Value;
// 		}
// 		else if (Mod.CalculationMethod == EStatusCalculationMethod::Multiply)
// 		{
// 			GetStatusRef(FinalStatus, Mod.Status) *= Mod.Value;
// 		}
// 		UE_LOG(LogTemp, Warning, TEXT("CalculateStatusFromModifiers"));
// 	}
// 	
// 	// 크리티컬 관련 제한
// 	FinalStatus.Dynamic_CritChance = FMath::Clamp(FinalStatus.Dynamic_CritChance, 0.0f, 0.85f); // 최소 0.0f(0%), 최대 1.0f(100%)로 제한
// 	FinalStatus.Dynamic_CritDamage = FMath::Max(1.0f, FinalStatus.Dynamic_CritDamage); // 1.0f(100%)로 최솟값 제한
// }

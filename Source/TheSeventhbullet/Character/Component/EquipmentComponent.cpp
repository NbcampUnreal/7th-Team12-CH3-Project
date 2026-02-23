#include "EquipmentComponent.h"


void UEquipmentComponent::EquipSoulGem(const FSoulGemInstance& SoulGem, int32 SlotIndex)
{
	if (SlotIndex < 0) return;
	
	if (!EquippedSoulGems.IsValidIndex(SlotIndex))
	{
		EquippedSoulGems.SetNum(SlotIndex + 1);
	}
	
	EquippedSoulGems[SlotIndex] = SoulGem;
	OnEquipmentChanged.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("OnEquipmentChanged Fire!"));
}

void UEquipmentComponent::CollectStatusModifiers(TArray<FStatusModifier>& Mod) const
{
	for (const FSoulGemInstance& Gem : EquippedSoulGems)
	{
		Mod.Append(Gem.StatusModifiers);
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::CollectStatusModifiers"));
	}
}

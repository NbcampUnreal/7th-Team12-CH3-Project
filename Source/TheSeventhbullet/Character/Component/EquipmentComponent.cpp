#include "EquipmentComponent.h"


void UEquipmentComponent::EquipSoulGem(const FSoulGemInstance& SoulGem, int32 SlotIndex)
{
	if (SlotIndex < 0) return;
	
	EquippedSoulGems[SlotIndex] = SoulGem;
	OnEquipmentChanged.Broadcast();
}

void UEquipmentComponent::CollectStatusModifiers(TArray<FStatusModifier>& Mod) const
{
	for (const FSoulGemInstance& Gem : EquippedSoulGems)
	{
		Mod.Append(Gem.StatusModifiers);
	}
}

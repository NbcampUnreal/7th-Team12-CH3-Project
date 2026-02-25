#include "StatusDamageModifier.h"
#include "Character/MainCharacter.h"

void UStatusDamageModifier::ModifyDamage(FDamageContext& Context)
{
	Super::ModifyDamage(Context);
	
	if (!Context.Attacker) return;
	
	AMainCharacter* Character = Cast<AMainCharacter>(Context.Attacker);
	if (!Character) return;
	
	Context.StatusDamage = Character->GetTotalStatus().Attack;
	Context.StatusCritChance = Character->GetTotalStatus().CriticalChance;
	Context.StatusCritDamage = Character->GetTotalStatus().CriticalHitChance;
		
	Context.CurrentDamage += Context.StatusDamage;
	Context.CurrentCritChance += Context.StatusCritChance;
	Context.CurrentCritDamage += Context.StatusCritDamage;
	
}

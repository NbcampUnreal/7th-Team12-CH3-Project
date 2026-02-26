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
	Context.StatusCritDamage = Character->GetTotalStatus().CriticalDamage;
		
	Context.CurrentDamage += Context.StatusDamage;
}

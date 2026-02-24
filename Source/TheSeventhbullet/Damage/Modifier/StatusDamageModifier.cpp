#include "StatusDamageModifier.h"
#include "Character/Component/GemStatusComponent.h"

void UStatusDamageModifier::ModifyDamage(FDamageContext& Context)
{
	Super::ModifyDamage(Context);
	
	if (!Context.Attacker) return;
	
	if (UGemStatusComponent* Status = Context.Attacker->FindComponentByClass<UGemStatusComponent>())
	{
		Context.StatusDamage = Status->GetFinalATK();
		Context.StatusCritChance = Status->GetFinalCritChance();
		Context.StatusCritDamage = Status->GetFinalCritDamage();
		
		Context.CurrentDamage += Context.StatusDamage;
		Context.CurrentCritChance += Context.StatusCritChance;
		Context.CurrentCritDamage += Context.StatusDamage;
	}
}

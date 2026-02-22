#include "StatusDamageModifier.h"
#include "Character/Component/StatusComponent.h"

void UStatusDamageModifier::ModifyDamage(FDamageContext& Context)
{
	Super::ModifyDamage(Context);
	
	if (!Context.Attacker) return;
	
	if (UStatusComponent* Status = Context.Attacker->FindComponentByClass<UStatusComponent>())
	{
		Context.StatusDamage = Status->GetFinalATK();
		Context.CurrentDamage += Context.StatusDamage;
	}
}

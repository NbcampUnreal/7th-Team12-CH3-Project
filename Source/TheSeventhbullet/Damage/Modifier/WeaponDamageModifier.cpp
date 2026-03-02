#include "WeaponDamageModifier.h"

void UWeaponDamageModifier::ModifyDamage(FDamageContext& Context)
{
	Super::ModifyDamage(Context);
	
	Context.CurrentCritChance += Context.WeaponCritChanceBalance;
	Context.CurrentCritDamage += Context.WeaponCritDamageBalance;
}

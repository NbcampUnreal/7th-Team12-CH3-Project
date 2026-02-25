#include "WeaponDamageModifier.h"

void UWeaponDamageModifier::ModifyDamage(FDamageContext& Context)
{
	Super::ModifyDamage(Context);
	
	Context.CurrentDamage += Context.WeaponDamage;
}

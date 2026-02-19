#include "WeaponDamageModifier.h"

#include "Weapon/WeaponBase.h"

void UWeaponDamageModifier::ModifyDamage(FDamageContext& Context)
{
	Super::ModifyDamage(Context);
	
	if (!Context.Weapon)
	{
		return;
	}
	
	Context.BaseDamage = Context.Weapon->GetBaseDamage();
	Context.CurrentDamage = Context.BaseDamage;
}

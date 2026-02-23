#pragma once

#include "CoreMinimal.h"
#include "DamageModifier.h"
#include "WeaponDamageModifier.generated.h"

UCLASS()
class THESEVENTHBULLET_API UWeaponDamageModifier : public UDamageModifier
{
	GENERATED_BODY()
	
public:
	virtual void ModifyDamage(FDamageContext& Context) override;
};

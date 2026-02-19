#pragma once

#include "CoreMinimal.h"
#include "Damage/DamageContext.h"
#include "UObject/Object.h"
#include "DamageModifier.generated.h"

UCLASS()
class THESEVENTHBULLET_API UDamageModifier : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void ModifyDamage(FDamageContext& Context);
};

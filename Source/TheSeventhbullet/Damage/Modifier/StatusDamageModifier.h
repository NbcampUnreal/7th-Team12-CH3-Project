#pragma once

#include "CoreMinimal.h"
#include "DamageModifier.h"
#include "StatusDamageModifier.generated.h"

UCLASS()
class THESEVENTHBULLET_API UStatusDamageModifier : public UDamageModifier
{
	GENERATED_BODY()
	
public:
	virtual void ModifyDamage(FDamageContext& Context) override;
};

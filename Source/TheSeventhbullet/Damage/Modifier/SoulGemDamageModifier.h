#pragma once

#include "CoreMinimal.h"
#include "DamageModifier.h"
#include "SoulGemDamageModifier.generated.h"

UCLASS()
class THESEVENTHBULLET_API USoulGemDamageModifier : public UDamageModifier
{
	GENERATED_BODY()
	
public:
	virtual void ModifyDamage(FDamageContext& Context) override;
};

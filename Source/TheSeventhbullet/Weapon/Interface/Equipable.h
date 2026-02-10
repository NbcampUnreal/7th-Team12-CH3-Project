#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Equipable.generated.h"

UINTERFACE()
class UEquipable : public UInterface
{
	GENERATED_BODY()
};

class THESEVENTHBULLET_API IEquipable
{
	GENERATED_BODY()
public:
	virtual void Equip(TObjectPtr<AActor> Owner) = 0;
	virtual void UnEquip() = 0;
};

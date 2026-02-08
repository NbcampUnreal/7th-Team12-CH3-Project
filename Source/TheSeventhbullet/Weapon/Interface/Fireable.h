#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Fireable.generated.h"

UINTERFACE(MinimalAPI)
class UFireable : public UInterface
{
	GENERATED_BODY()
};

class THESEVENTHBULLET_API IFireable
{
	GENERATED_BODY()
public:
	virtual void Fire() = 0;
	virtual void Reload() = 0;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MaterialBase.generated.h"

UCLASS()
class THESEVENTHBULLET_API AMaterialBase : public AActor
{
	GENERATED_BODY()

public:
	AMaterialBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

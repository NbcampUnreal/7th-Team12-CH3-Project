#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "CraftingComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UCraftingComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	UCraftingComponent();

protected:
	virtual void BeginPlay() override;
};

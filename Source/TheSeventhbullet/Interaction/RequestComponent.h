#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "RequestComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API URequestComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	URequestComponent();
public:
	virtual void BeginInteract(AActor* Interactor);
	virtual void ProgressInteract(AActor* Interactor);
	virtual void EndInteract(AActor* Interactor);
protected:
	virtual void BeginPlay() override;

};

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionActor.h"
#include "SaveComponent.h"
#include "SaveActor.generated.h"

UCLASS()
class THESEVENTHBULLET_API ASaveActor : public ABaseInteractionActor
{
	GENERATED_BODY()

public:
	ASaveActor();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Bed")
	TObjectPtr<USaveComponent> SaveComponent;
public:
	virtual void Interact(AActor* Interactor) override;
	
	UFUNCTION(BlueprintPure, Category = "Interaction|Bed")
	USaveComponent* GetSaveComponent() const {return SaveComponent;}
};

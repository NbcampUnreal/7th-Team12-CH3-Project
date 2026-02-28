#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "System/GameInstance/MainGameInstance.h"
#include "SaveComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API USaveComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	USaveComponent();

protected:
	virtual void BeginPlay() override;
	
	virtual void BeginInteract(AActor* Interactor) override;
	virtual void ProgressInteract(AActor* Interactor) override;
	virtual void EndInteract(AActor* Interactor) override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Bed|UI")
	
private:
	UMainGameInstance* GI = nullptr;
};

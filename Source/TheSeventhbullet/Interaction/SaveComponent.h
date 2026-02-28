#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "System/GameInstance/MainGameInstance.h"
#include "UI/SaveWidget.h"
#include "SaveComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API USaveComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	USaveComponent();
	virtual void BeginInteract(AActor* Interactor) override;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Bed|UI")
	TSubclassOf<USaveWidget> SaveWidgetClass;
	
private:
	UPROPERTY()
	TObjectPtr<USaveWidget> SaveWidget;
	
	bool bIsTransitioning = false;
	
	UFUNCTION()
	void OnSleepAnimComplete();
	
	UFUNCTION()
	void OnWakeAnimComplete();
	
	void HandleNextDay();
	void SetPlayerInputEnabled(AActor* Interactor, bool bEnabled);

	
private:
	UMainGameInstance* GI = nullptr;
};

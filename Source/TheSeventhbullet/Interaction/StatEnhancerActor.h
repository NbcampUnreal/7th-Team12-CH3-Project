#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionActor.h"
#include "UI/StatEnhanceWidget.h"
#include "StatEnhancerActor.generated.h"

UCLASS()
class THESEVENTHBULLET_API AStatEnhancerActor : public ABaseInteractionActor
{
	GENERATED_BODY()

public:
	AStatEnhancerActor();
	
	virtual void Interact(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

private:
	void OpenEnhancerUI();
	void CloseEnhancerUI();
	
	UFUNCTION()
	void OnStatUpgraded();
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Enhance",
	meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatEnhancerComponent> StatEnhancerComponent;
	
	UPROPERTY()
	TObjectPtr<UStatEnhanceWidget> CachedWidget;
	
};

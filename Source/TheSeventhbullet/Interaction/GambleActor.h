#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionActor.h"
#include "GambleComponent.h"
#include "GambleActor.generated.h"

UCLASS()
class THESEVENTHBULLET_API AGambleActor : public ABaseInteractionActor
{
	GENERATED_BODY()

public:
	AGambleActor();
	
	virtual void Interact(AActor* Interactor) override;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gamble")
	TObjectPtr<UGambleComponent> GambleComponent;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> CachedInteractor;
	
	UFUNCTION()
	void OnInteractionResult(const FGambleResultData& GambleData);
	
	UFUNCTION()
	void OnGambleFinished(bool bIsWin);
	
	UFUNCTION(BlueprintCallable, Category = "Gamble")
	void OnDialogueConfirmed();
	
	UFUNCTION(BlueprintCallable, Category = "Gamble")
	void OnDialogueCancelled();
	
	void GiveReward();
	void ApplyPenalty();
};

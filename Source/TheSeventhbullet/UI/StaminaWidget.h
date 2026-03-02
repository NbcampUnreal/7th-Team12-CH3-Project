#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaWidget.generated.h"

class UImage;
class AMainCharacter;

UCLASS()
class THESEVENTHBULLET_API UStaminaWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Stamina")
	void UpdatePercentage(float Percent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> StaminaFill;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Stamina")
	TObjectPtr<UMaterialInterface> StaminaBarMaterial;

private:
	UFUNCTION()
	void OnStaminaChanged(float CurrentStamina, float MaxStamina);

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMaterial;

	UPROPERTY()
	TObjectPtr<AMainCharacter> CachedCharacter;
};

#pragma once

#include "CoreMinimal.h"
#include "WeaponSlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "Character/MainCharacter.h"
#include "WeaponSelectWidget.generated.h"

UCLASS()
class THESEVENTHBULLET_API UWeaponSelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,Category = "UI|Weapon")
	void InitWeaponSelect(AMainCharacter* InCharacter);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWeaponSlotWidget> PistolSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWeaponSlotWidget> RifleSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWeaponSlotWidget> ShotGunSlot;

private:
	void OnSlotSelected(UWeaponSlotWidget* SelectedSlot);

	TArray<UWeaponSlotWidget*> AllSlots;
};

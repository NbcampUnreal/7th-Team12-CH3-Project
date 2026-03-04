#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/MainCharacter.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/StreamableManager.h"
#include "WeaponSlotWidget.generated.h"

class UTextBlock;
class UWeaponSlotWidget;
class AMainCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponSlotSelected, UWeaponSlotWidget*);

UCLASS()
class THESEVENTHBULLET_API UWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitSlot(AMainCharacter* InCharacter);
	void SetSelected(bool bSelected);
	FPrimaryAssetId GetWeaponAssetId() const { return WeaponAssetId; }

	FOnWeaponSlotSelected OnSlotSelected;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FPrimaryAssetId WeaponAssetId;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UTexture2D> WeaponTexture;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FText WeaponNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponName;
private:
	UFUNCTION()
	void OnSlotButtonClicked();

	void OnWeaponDataLoaded();

	UPROPERTY()
	TObjectPtr<AMainCharacter> OwningCharacter;

	TSharedPtr<FStreamableHandle> StreamableHandle;
};

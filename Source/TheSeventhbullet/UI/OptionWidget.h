#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionWidget.generated.h"

class USlider;
class UTextBlock;
class UComboBoxString;
class UButton;

UCLASS()
class THESEVENTHBULLET_API UOptionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// Volume Sliders
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MasterVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> BGMVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SFXVolumeSlider;

	// Volume Value Texts
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MasterValueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> BGMValueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SFXValueText;

	// Resolution & Window Mode
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ResolutionComboBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> WindowModeComboBox;

	// Buttons
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

private:
	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnBGMVolumeChanged(float Value);

	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	UFUNCTION()
	void OnApplyClicked();

	UFUNCTION()
	void OnBackClicked();

	void UpdateVolumeText(UTextBlock* Text, float Value);
	void InitResolutionOptions();
	void InitWindowModeOptions();
	void LoadCurrentSettings();

	struct FScreenResolution
	{
		int32 Width;
		int32 Height;
		FString ToString() const { return FString::Printf(TEXT("%d x %d"), Width, Height); }
	};

	TArray<FScreenResolution> Resolutions;
};

#include "OptionWidget.h"
#include "UITags.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "GameFramework/GameUserSettings.h"
#include "Manager/UIManager.h"
#include "Manager/SoundManager.h"

void UOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Slider callbacks
	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UOptionWidget::OnMasterVolumeChanged);
	}
	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->OnValueChanged.AddDynamic(this, &UOptionWidget::OnBGMVolumeChanged);
	}
	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->OnValueChanged.AddDynamic(this, &UOptionWidget::OnSFXVolumeChanged);
	}

	// Button callbacks
	if (ApplyButton)
	{
		ApplyButton->OnClicked.AddDynamic(this, &UOptionWidget::OnApplyClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UOptionWidget::OnBackClicked);
	}

	InitResolutionOptions();
	InitWindowModeOptions();
	LoadCurrentSettings();
}

void UOptionWidget::InitResolutionOptions()
{
	Resolutions.Empty();
	Resolutions.Add({1280, 720});
	Resolutions.Add({1920, 1080});
	Resolutions.Add({2560, 1440});
	Resolutions.Add({3840, 2160});

	if (ResolutionComboBox)
	{
		ResolutionComboBox->ClearOptions();
		for (const auto& Res : Resolutions)
		{
			ResolutionComboBox->AddOption(Res.ToString());
		}
	}
}

void UOptionWidget::InitWindowModeOptions()
{
	if (WindowModeComboBox)
	{
		WindowModeComboBox->ClearOptions();
		WindowModeComboBox->AddOption(TEXT("Fullscreen"));
		WindowModeComboBox->AddOption(TEXT("Windowed Fullscreen"));
		WindowModeComboBox->AddOption(TEXT("Windowed"));
	}
}

void UOptionWidget::LoadCurrentSettings()
{
	// Load volume from SoundManager
	USoundManager* SoundMgr = USoundManager::Get(this);
	if (SoundMgr)
	{
		if (MasterVolumeSlider)
		{
			MasterVolumeSlider->SetValue(SoundMgr->GetMasterVolume());
		}
		if (BGMVolumeSlider)
		{
			BGMVolumeSlider->SetValue(SoundMgr->GetBGMVolume());
		}
		if (SFXVolumeSlider)
		{
			SFXVolumeSlider->SetValue(SoundMgr->GetSFXVolume());
		}

		UpdateVolumeText(MasterValueText, SoundMgr->GetMasterVolume());
		UpdateVolumeText(BGMValueText, SoundMgr->GetBGMVolume());
		UpdateVolumeText(SFXValueText, SoundMgr->GetSFXVolume());
	}

	// Load resolution & window mode from GameUserSettings
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		const FIntPoint CurrentRes = Settings->GetScreenResolution();

		if (ResolutionComboBox)
		{
			for (int32 i = 0; i < Resolutions.Num(); i++)
			{
				if (Resolutions[i].Width == CurrentRes.X && Resolutions[i].Height == CurrentRes.Y)
				{
					ResolutionComboBox->SetSelectedIndex(i);
					break;
				}
			}
		}

		if (WindowModeComboBox)
		{
			const EWindowMode::Type CurrentMode = Settings->GetFullscreenMode();
			switch (CurrentMode)
			{
			case EWindowMode::Fullscreen:
				WindowModeComboBox->SetSelectedIndex(0);
				break;
			case EWindowMode::WindowedFullscreen:
				WindowModeComboBox->SetSelectedIndex(1);
				break;
			case EWindowMode::Windowed:
				WindowModeComboBox->SetSelectedIndex(2);
				break;
			}
		}
	}
}

void UOptionWidget::UpdateVolumeText(UTextBlock* Text, float Value)
{
	if (Text)
	{
		Text->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Value * 100.f))));
	}
}

void UOptionWidget::OnMasterVolumeChanged(float Value)
{
	UpdateVolumeText(MasterValueText, Value);
	if (USoundManager* SoundMgr = USoundManager::Get(this))
	{
		SoundMgr->SetMasterVolume(Value);
	}
}

void UOptionWidget::OnBGMVolumeChanged(float Value)
{
	UpdateVolumeText(BGMValueText, Value);
	if (USoundManager* SoundMgr = USoundManager::Get(this))
	{
		SoundMgr->SetBGMVolume(Value);
	}
}

void UOptionWidget::OnSFXVolumeChanged(float Value)
{
	UpdateVolumeText(SFXValueText, Value);
	if (USoundManager* SoundMgr = USoundManager::Get(this))
	{
		SoundMgr->SetSFXVolume(Value);
	}
}

void UOptionWidget::OnApplyClicked()
{
	// Save audio settings
	if (USoundManager* SoundMgr = USoundManager::Get(this))
	{
		SoundMgr->SaveAudioSettings();
	}

	// Apply resolution & window mode
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		// Resolution
		if (ResolutionComboBox)
		{
			const int32 SelectedIdx = ResolutionComboBox->GetSelectedIndex();
			if (Resolutions.IsValidIndex(SelectedIdx))
			{
				Settings->SetScreenResolution(FIntPoint(Resolutions[SelectedIdx].Width, Resolutions[SelectedIdx].Height));
			}
		}

		// Window Mode
		if (WindowModeComboBox)
		{
			const int32 ModeIdx = WindowModeComboBox->GetSelectedIndex();
			switch (ModeIdx)
			{
			case 0:
				Settings->SetFullscreenMode(EWindowMode::Fullscreen);
				break;
			case 1:
				Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
				break;
			case 2:
				Settings->SetFullscreenMode(EWindowMode::Windowed);
				break;
			}
		}

		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
}

void UOptionWidget::OnBackClicked()
{
	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		UIMgr->Close(UITags::Option);
	}
}

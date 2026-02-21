#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->SetIsEnabled(false);
		ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueClicked);
		SetupButtonHover(ContinueButton);
	}

	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGameClicked);
		SetupButtonHover(NewGameButton);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsClicked);
		SetupButtonHover(SettingsButton);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
		SetupButtonHover(QuitButton);
	}
}

void UMainMenuWidget::SetupButtonHover(UButton* Button)
{
	if (!Button) return;

	UTextBlock* TextBlock = FindChildTextBlock(Button);
	if (!TextBlock) return;

	ButtonTextMap.Add(Button, TextBlock);
	TextBlock->SetColorAndOpacity(NormalTextColor);

	Button->OnHovered.AddDynamic(this, &UMainMenuWidget::OnAnyButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnAnyButtonUnhovered);
}

UTextBlock* UMainMenuWidget::FindChildTextBlock(UWidget* Parent)
{
	UPanelWidget* Panel = Cast<UPanelWidget>(Parent);
	if (!Panel) return nullptr;

	for (int32 i = 0; i < Panel->GetChildrenCount(); i++)
	{
		UWidget* Child = Panel->GetChildAt(i);
		if (UTextBlock* Text = Cast<UTextBlock>(Child))
		{
			return Text;
		}
		if (UTextBlock* Found = FindChildTextBlock(Child))
		{
			return Found;
		}
	}
	return nullptr;
}

void UMainMenuWidget::OnAnyButtonHovered()
{
	for (auto& Pair : ButtonTextMap)
	{
		if (Pair.Key && Pair.Key->IsHovered() && Pair.Value)
		{
			Pair.Value->SetColorAndOpacity(HoveredTextColor);
		}
	}
}

void UMainMenuWidget::OnAnyButtonUnhovered()
{
	for (auto& Pair : ButtonTextMap)
	{
		if (Pair.Key && !Pair.Key->IsHovered() && Pair.Value)
		{
			Pair.Value->SetColorAndOpacity(NormalTextColor);
		}
	}
}

void UMainMenuWidget::OnContinueClicked()
{

}

void UMainMenuWidget::OnNewGameClicked()
{
	
}

void UMainMenuWidget::OnSettingsClicked()
{

}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "System/GameInstance/MainGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		UMainGameInstance* GI = UMainGameInstance::Get(this);
		ContinueButton->SetIsEnabled(GI && GI->DoesSaveExist());
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

void UMainMenuWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (InVisibility == ESlateVisibility::Visible && ContinueButton)
	{
		UMainGameInstance* GI = UMainGameInstance::Get(this);
		ContinueButton->SetIsEnabled(GI && GI->DoesSaveExist());
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
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (GI)
	{
		GI->LoadAsyncSaveData();
		GI->GameStartMapLoad();
	}
}

void UMainMenuWidget::OnNewGameClicked()
{
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (GI)
	{
		GI->StartNewGame();
	}
}

void UMainMenuWidget::OnSettingsClicked()
{

}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}
#include "EscMenuWidget.h"
#include "UITags.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Manager/UIManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UEscMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UEscMenuWidget::OnResumeClicked);
		SetupButtonHover(ResumeButton);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UEscMenuWidget::OnSettingsClicked);
		SetupButtonHover(SettingsButton);
	}

	if (BackToMenuButton)
	{
		BackToMenuButton->OnClicked.AddDynamic(this, &UEscMenuWidget::OnBackToMenuClicked);
		SetupButtonHover(BackToMenuButton);
	}

	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UEscMenuWidget::OnQuitGameClicked);
		SetupButtonHover(QuitGameButton);
	}
}

// --- Button Hover ---

void UEscMenuWidget::SetupButtonHover(UButton* Button)
{
	if (!Button) return;

	UTextBlock* TextBlock = FindChildTextBlock(Button);
	if (!TextBlock) return;

	ButtonTextMap.Add(Button, TextBlock);
	TextBlock->SetColorAndOpacity(NormalTextColor);

	Button->OnHovered.AddDynamic(this, &UEscMenuWidget::OnAnyButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &UEscMenuWidget::OnAnyButtonUnhovered);
}

UTextBlock* UEscMenuWidget::FindChildTextBlock(UWidget* Parent)
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

void UEscMenuWidget::OnAnyButtonHovered()
{
	for (auto& Pair : ButtonTextMap)
	{
		if (Pair.Key && Pair.Key->IsHovered() && Pair.Value)
		{
			Pair.Value->SetColorAndOpacity(HoveredTextColor);
		}
	}
}

void UEscMenuWidget::OnAnyButtonUnhovered()
{
	for (auto& Pair : ButtonTextMap)
	{
		if (Pair.Key && !Pair.Key->IsHovered() && Pair.Value)
		{
			Pair.Value->SetColorAndOpacity(NormalTextColor);
		}
	}
}

// --- Button Actions ---

void UEscMenuWidget::OnResumeClicked()
{
	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		UIMgr->Toggle(UITags::EscMenu);
	}
}

void UEscMenuWidget::OnSettingsClicked()
{
	// TODO: Settings UI Push
}

void UEscMenuWidget::OnBackToMenuClicked()
{
	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		UIMgr->PopAll();
	}
	UGameplayStatics::OpenLevel(this, MainMenuMapName);
}

void UEscMenuWidget::OnQuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}
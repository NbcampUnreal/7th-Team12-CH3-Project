#include "GambleBettingDialogueWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UGambleBettingDialogueWidget::SetDialogue(const FText& InText)
{
	if (DialogueText)
		DialogueText->SetText(InText);
}

void UGambleBettingDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Super::NativeConstruct();
	if (DialogueText)
		DialogueText->SetText(DefaultDialogueText);
	if (EasyBettingButton)
		EasyBettingButton->OnClicked.AddDynamic(this, &UGambleBettingDialogueWidget::OnEasyBettingClicked);
	if (NormalBettingButton)
		NormalBettingButton->OnClicked.AddDynamic(this, &UGambleBettingDialogueWidget::OnNormalBettingClicked);
	if (HardBettingButton)
		HardBettingButton->OnClicked.AddDynamic(this, &UGambleBettingDialogueWidget::OnHardBettingClicked);
}

void UGambleBettingDialogueWidget::OnEasyBettingClicked()
{
	OnBettingChose.Broadcast(500);
}

void UGambleBettingDialogueWidget::OnNormalBettingClicked()
{
	OnBettingChose.Broadcast(1000);
}

void UGambleBettingDialogueWidget::OnHardBettingClicked()
{
	OnBettingChose.Broadcast(2000);
}

#include "GambleDialogueWidget.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UGambleDialogueWidget::SetDialogue(const FText& InText)
{
	if (DialogueText)
		DialogueText->SetText(InText);
}

void UGambleDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (DialogueText)
		DialogueText->SetText(DefaultDialogueText);
	if (ConfirmButton)
		ConfirmButton->OnClicked.AddDynamic(this, &UGambleDialogueWidget::OnConfirmClicked);
	if (CancelButton)
		CancelButton->OnClicked.AddDynamic(this, &UGambleDialogueWidget::OnCancelClicked);
}	

void UGambleDialogueWidget::OnConfirmClicked()
{
	OnConfirmed.Broadcast();
}

void UGambleDialogueWidget::OnCancelClicked()
{
	OnCancelled.Broadcast();
}

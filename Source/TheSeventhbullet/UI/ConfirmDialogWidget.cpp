#include "ConfirmDialogWidget.h"
#include "UITags.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Manager/UIManager.h"

void UConfirmDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton && !ConfirmButton->OnClicked.IsAlreadyBound(this, &UConfirmDialogWidget::OnConfirmClicked))
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UConfirmDialogWidget::OnConfirmClicked);
	}

	if (CancelButton && !CancelButton->OnClicked.IsAlreadyBound(this, &UConfirmDialogWidget::OnCancelClicked))
	{
		CancelButton->OnClicked.AddDynamic(this, &UConfirmDialogWidget::OnCancelClicked);
	}
}

void UConfirmDialogWidget::ShowDialog(const FText& Message, FOnConfirmDialogResult InCallback)
{
	ResultCallback = InCallback;

	if (DialogueText)
	{
		DialogueText->SetText(Message);
	}
}

void UConfirmDialogWidget::OnConfirmClicked()
{
	CloseAndRespond(true);
}

void UConfirmDialogWidget::OnCancelClicked()
{
	CloseAndRespond(false);
}

void UConfirmDialogWidget::CloseAndRespond(bool bConfirmed)
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::ConfirmDialog);
	}

	if (ResultCallback.IsBound())
	{
		ResultCallback.Execute(bConfirmed);
		ResultCallback.Unbind();
	}
}

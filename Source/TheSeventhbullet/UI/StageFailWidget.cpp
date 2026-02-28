#include "StageFailWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Manager/UIManager.h"
#include "UITags.h"
#include "System/MainGameMode.h"

void UStageFailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ReturnButton && !ReturnButton->OnClicked.IsAlreadyBound(this, &UStageFailWidget::OnReturnClicked))
	{
		ReturnButton->OnClicked.AddDynamic(this, &UStageFailWidget::OnReturnClicked);
	}

	if (ShowAnimation)
	{
		PlayAnimation(ShowAnimation);
	}
}

void UStageFailWidget::SetFailReason(EStageResult Result)
{
	CachedResult = Result;

	if (!FailReasonText) return;

	switch (Result)
	{
	case EStageResult::TimeOver:
		FailReasonText->SetText(FText::FromString(TEXT("Time Over")));
		if (ReturnButtonText)
		{
			ReturnButtonText->SetText(FText::FromString(TEXT("Return to Town")));
		}
		break;
	case EStageResult::PlayerDead:
		FailReasonText->SetText(FText::FromString(TEXT("You Died")));
		if (ReturnButtonText)
		{
			ReturnButtonText->SetText(FText::FromString(TEXT("Game Over")));
		}
		break;
	default:
		FailReasonText->SetText(FText::FromString(TEXT("Stage Failed")));
		if (ReturnButtonText)
		{
			ReturnButtonText->SetText(FText::FromString(TEXT("Return to Town")));
		}
		break;
	}
}

void UStageFailWidget::OnReturnClicked()
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;

	if (CachedResult == EStageResult::PlayerDead)
	{
		GM->ReturnToMainMenu();
	}
	else
	{
		GM->ReturnToTown();
	}
}

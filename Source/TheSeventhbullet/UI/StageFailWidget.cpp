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
}

void UStageFailWidget::SetFailReason(EStageResult Result)
{
	if (!FailReasonText) return;

	switch (Result)
	{
	case EStageResult::TimeOver:
		FailReasonText->SetText(FText::FromString(TEXT("시간 초과")));
		break;
	case EStageResult::PlayerDead:
		FailReasonText->SetText(FText::FromString(TEXT("사망")));
		break;
	default:
		FailReasonText->SetText(FText::FromString(TEXT("스테이지 실패")));
		break;
	}
}

void UStageFailWidget::OnReturnClicked()
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->ReturnToTown();
	}
}

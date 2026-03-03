#include "TownHUDWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Manager/SyncDataManager.h"
#include "System/MainGameMode.h"
#include "System/GameInstance/MainGameInstance.h"

void UTownHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->OnTownPhaseChanged.AddDynamic(this, &UTownHUDWidget::OnTownPhaseChanged);
		PendingPhase = GM->GetTownPhase();
	}

	ApplyPhaseContent();
}

void UTownHUDWidget::NativeDestruct()
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->OnTownPhaseChanged.RemoveDynamic(this, &UTownHUDWidget::OnTownPhaseChanged);
	}

	Super::NativeDestruct();
}

void UTownHUDWidget::OnTownPhaseChanged(ETownPhase NewPhase)
{
	if (PendingPhase == NewPhase) return;

	PendingPhase = NewPhase;
	PlayHide();
}

void UTownHUDWidget::OnHideAnimFinished()
{
	ApplyPhaseContent();
	PlayShow();
}

void UTownHUDWidget::ApplyPhaseContent()
{
	if (PhaseMessageText)
	{
		PhaseMessageText->SetText(BuildPhaseMessage());
	}

	if (RequestImage)
	{
		if (PendingPhase == ETownPhase::RequestAccepted)
		{
			RequestImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			RequestImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FText UTownHUDWidget::BuildPhaseMessage() const
{
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	int32 Day = GI ? GI->CurrentDay : 1;

	AMainGameMode* GM = AMainGameMode::Get(this);

	if (PendingPhase == ETownPhase::RequestAccepted && GM)
	{
		int32 RequestID = GM->GetCurrentRequestID();
		USyncDataManager* DataManager = USyncDataManager::Get(this);
		if (DataManager && RequestID != INDEX_NONE)
		{
			FRequestRowData Data = DataManager->GetRequestData(RequestID);
			return FText::Format(
				NSLOCTEXT("TownHUD", "RequestAccepted", "{0}  |  보상: {1}G  |  난이도: {2}"),
				FText::FromName(Data.RequestName),
				FText::AsNumber(Data.RequestGold),
				FText::AsNumber(Data.RequestLevel)
			);
		}
	}

	switch (PendingPhase)
	{
	case ETownPhase::AcceptRequest:
		return FText::Format(
			NSLOCTEXT("TownHUD", "AcceptRequest", "Day {0} : 의뢰를 받으세요"),
			FText::AsNumber(Day));
	case ETownPhase::WaitForNextDay:
		return FText::Format(
			NSLOCTEXT("TownHUD", "WaitForNextDay", "Day {0} : 다음날을 기다리세요"),
			FText::AsNumber(Day));
	default:
		return FText::GetEmpty();
	}
}

void UTownHUDWidget::PlayShow()
{
	if (ShowAnimation)
	{
		PlayAnimation(ShowAnimation);
	}
}

void UTownHUDWidget::PlayHide()
{
	if (HideAnimation)
	{
		FWidgetAnimationDynamicEvent EndDelegate;
		EndDelegate.BindDynamic(this, &UTownHUDWidget::OnHideAnimFinished);
		BindToAnimationFinished(HideAnimation, EndDelegate);
		PlayAnimation(HideAnimation);
	}
	else
	{
		OnHideAnimFinished();
	}
}

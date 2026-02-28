#include "ResultWaveState.h"

#include "Manager/UIManager.h"
#include "UI/UITags.h"
#include "UI/StageSuccessWidget.h"
#include "UI/StageFailWidget.h"

void UResultWaveState::Enter()
{
	Super::Enter();
	AMainGameMode* GM = GetGameMode();
	if (!GM) return;

	EStageResult Result = GM->GetStageResult();

	GM->CleanupAllMonsters();

	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr) return;

	UIMgr->Close(UITags::HUD);
	UIMgr->Close(UITags::Crosshair);

	switch (Result)
	{
	case EStageResult::Success:
		{
			UUserWidget* Widget = UIMgr->Open(UITags::StageSuccess);
			if (UStageSuccessWidget* SuccessWidget = Cast<UStageSuccessWidget>(Widget))
			{
				SuccessWidget->SetRewards(GM->GetStageRewardItems());
			}
		}
		break;
	case EStageResult::TimeOver:
		{
			UUserWidget* Widget = UIMgr->Open(UITags::StageFail);
			if (UStageFailWidget* FailWidget = Cast<UStageFailWidget>(Widget))
			{
				FailWidget->SetFailReason(EStageResult::TimeOver);
			}
		}
		break;
	case EStageResult::PlayerDead:
		{
			UUserWidget* Widget = UIMgr->Open(UITags::StageFail);
			if (UStageFailWidget* FailWidget = Cast<UStageFailWidget>(Widget))
			{
				FailWidget->SetFailReason(EStageResult::PlayerDead);
			}
		}
		break;
	default:
		break;
	}

	bWaitingForInput = true;
}

void UResultWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 결산 화면에서 버튼 입력 기다림
	// 실제 UI 버튼의 OnClicked에서 GM->ReturnToTown()을 호출
}

void UResultWaveState::Exit()
{
	Super::Exit();

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::StageSuccess);
		UIMgr->Close(UITags::StageFail);
	}

	bWaitingForInput = false;
}

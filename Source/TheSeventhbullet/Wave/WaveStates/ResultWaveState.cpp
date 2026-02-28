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

	UE_LOG(LogTemp, Warning, TEXT("[ResultWaveState] StageResult=%d"), (int32)Result);

	switch (Result)
	{
	case EStageResult::Success:
		{
			const TArray<FDroppedMaterialsData>& Rewards = GM->GetStageRewardItems();
			UE_LOG(LogTemp, Warning, TEXT("[ResultWaveState] Success - RewardItems Num=%d"), Rewards.Num());
			for (int32 i = 0; i < Rewards.Num(); i++)
			{
				UE_LOG(LogTemp, Warning, TEXT("[ResultWaveState]   [%d] Material=%s, Count=%d"),
					i, *Rewards[i].Material.GetAssetName(), Rewards[i].Count);
			}

			UUserWidget* Widget = UIMgr->Open(UITags::StageSuccess);
			UE_LOG(LogTemp, Warning, TEXT("[ResultWaveState] Open StageSuccess Widget=%s"), Widget ? *Widget->GetName() : TEXT("NULL"));

			if (UStageSuccessWidget* SuccessWidget = Cast<UStageSuccessWidget>(Widget))
			{
				SuccessWidget->SetRewards(Rewards);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[ResultWaveState] Cast to UStageSuccessWidget FAILED"));
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

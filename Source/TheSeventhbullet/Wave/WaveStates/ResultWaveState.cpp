#include "ResultWaveState.h"

#include "Manager/UIManager.h"

void UResultWaveState::Enter()
{
	Super::Enter();
	AMainGameMode* GM = GetGameMode();
	if (!GM) return;
	
	EStageResult Result = GM->GetStageResult();
	
	GM->CleanupAllMonsters();
	
	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr) return;
	
	switch (Result)
	{
	case EStageResult::Success:
		// TODO 영섭 : 보상 정산 데이터 전달
		break;
	case EStageResult::TimeOver:
		// TODO 영섭 : 시간 초과 메세지 전달
		break;
	case EStageResult::PlayerDead:
		// TODO 영섭 : "사망" 메세지 전달, 부활
		break;
	default:
		break;
	}
	
	bWaitingForInput = true;
}

void UResultWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//결산 화면에서 버튼 입력 기다림
	//실제 UI버튼의 Onclick에서 GM->ReturnToTown()을 호출
	AMainGameMode* GM = GetGameMode();
	GM->ReturnToTown();
}

void UResultWaveState::Exit()
{
	Super::Exit();
	UE_LOG(LogTemp,Error,TEXT("RESULT"));
	//TODO 영섭 : UI 끄기
	// UUIManager* UIMgr = UUIManager::Get(this);
	// if (UIMgr)
	// {
	// 	UIMgr->HideByTag(UITags::StageSuccessPanel);
	// 	UIMgr->HideByTag(UITags::StageFailPanel);
	// }
	
	bWaitingForInput = false;
}

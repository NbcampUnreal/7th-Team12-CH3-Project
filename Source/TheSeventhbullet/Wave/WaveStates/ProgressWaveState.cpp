// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressWaveState.h"

#include "Manager/UIManager.h"
#include "UI/UITags.h"
#include "UI/MainHUDWidget.h"

void UProgressWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Progress Wave"));

	GM = GetGameMode();
	if (!GM) return;

	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		if (UMainHUDWidget* HUD = Cast<UMainHUDWidget>(UIMgr->GetWidget(UITags::HUD)))
		{
			HUD->ShowWaveInfo(GM->GetCurrentWaveIndex() + 1, GM->GetStageRemainingTime());
			HUD->UpdateMonsterCount(GM->GetAliveMonsterCount());
		}
	}
	
	if (GM->IsBossWave())
	{
		GM->OnBossWaveStarted.Broadcast();
	}
	
}

void UProgressWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GM) return;
	
	//스폰 로직
	GM->UpdateSpawnLogic(DeltaTime);
	
	//스테이지 타이머
	GM->UpdateStageTimer(DeltaTime);

	// HUD 업데이트
	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		if (UMainHUDWidget* HUD = Cast<UMainHUDWidget>(UIMgr->GetWidget(UITags::HUD)))
		{
			HUD->UpdateWaveTimer(GM->GetStageRemainingTime());
			HUD->UpdateMonsterCount(GM->GetAliveMonsterCount());
		}
	}

	//시간 초과 체크
	if (GM->IsStageTimeOver())
	{
		GM->SetStageResult(EStageResult::TimeOver);
		ChangeState(EWaveState::StageResult);
		return;
	}
	if (GM->IsWaveClear())
	{
		if (GM->IsCurrentWaveManualTrigger() && !GM->IsBossDead())
		{
			return;
		}
		ChangeState(EWaveState::End);
	}
}

void UProgressWaveState::Exit()
{
	Super::Exit();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressWaveState.h"

#include "Manager/UIManager.h"

void UProgressWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Progress Wave"));
	
	GM = GetGameMode();
	if (!GM) return;

	//TODO 영섭 : 타이머, 남은 몬스터 수, 웨이브 번호 등등 UI띄우기
	// UUIManager* UIMgr = UUIManager::Get(this);
	// if (UIMgr)
	// {
	// 	UIMgr->ShowByTag(UITags::StageCountdown);
	// }
}

void UProgressWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GM) return;
	
	//스폰 로직
	GM->UpdateSpawnLogic(DeltaTime);
	
	//스테이지 타이머
	GM->UpdateStageTimer(DeltaTime);
	
	//시간 초과 체크
	if (GM->IsStageTimeOver())
	{
		GM->SetStageResult(EStageResult::TimeOver);
		ChangeState(EWaveState::StageResult);
		return;
	}
	if (GM->IsWaveClear())
	{
		ChangeState(EWaveState::End);
	}
}

void UProgressWaveState::Exit()
{
	Super::Exit();
}

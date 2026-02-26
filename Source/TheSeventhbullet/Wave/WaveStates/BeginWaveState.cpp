// Fill out your copyright notice in the Description page of Project Settings.


#include "BeginWaveState.h"

#include "Manager/UIManager.h"

void UBeginWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Begin Wave"));
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;
	
	GM->SetupCurrentWaveData();
	DelayTimer = GM->GetWaveStartDelay();
	
	//TODO 영섭 : 위젯에 웨이브 번호와 카운트다운 시간 전달
	//예시
	// UUIManager* UIMgr = UUIManager::Get(this);
	// if (UIMgr)
	// {
	// 	UIMgr->ShowByTag(UITags::WaveStartCountdown);
	// 	CountdownWidget->SetWaveNumber(GM->GetCurrentWaveIndex() + 1);
	// 	CountdownWidget->StartCountdown(DelayTimer);
	// }
}

void UBeginWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DelayTimer -= DeltaTime;
	
	if (DelayTimer <= 0.0f)
	{
		ChangeState(EWaveState::Progress);
	}
}

void UBeginWaveState::Exit()
{
	Super::Exit();
	
	//TODO 영섭 : 카운트 다운 UI 숨김
	// UUIManager* UIMgr = UUIManager::Get(this);
	// if (UIMgr)
	// {
	// 	UIMgr->HideByTag(UITags::WaveStartCountdown);
	// }
	
}

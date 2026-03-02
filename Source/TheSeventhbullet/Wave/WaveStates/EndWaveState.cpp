// Fill out your copyright notice in the Description page of Project Settings.


#include "EndWaveState.h"

#include "Manager/UIManager.h"
#include "UI/UITags.h"
#include "UI/MainHUDWidget.h"

void UEndWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("End Wave"));

	ClearDelayTimer = 2.0f;
	bHasDecided = false;

	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		if (UMainHUDWidget* HUD = Cast<UMainHUDWidget>(UIMgr->GetWidget(UITags::HUD)))
		{
			HUD->ShowWaveClear();
		}
	}
}

void UEndWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasDecided) return;
	ClearDelayTimer -= DeltaTime;
	if (ClearDelayTimer > 0.0f) return;

	AMainGameMode* GM = GetGameMode();
	if (!GM) return;

	
	bHasDecided = true;
	GM->IncreaseCurrentWaveIndex();
	
	if (GM->HasNextWave())
	{
		if (GM->IsBossWave())
			ChangeState(EWaveState::Begin);
		else
			ChangeState(EWaveState::Intermission);
	}
	else
	{
		UE_LOG(LogTemp,Error, TEXT("No Wave Request Success"));
		GM->SetStageResult(EStageResult::Success);
		ChangeState(EWaveState::StageResult);
	}
}

void UEndWaveState::Exit()
{
	Super::Exit();

	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		if (UMainHUDWidget* HUD = Cast<UMainHUDWidget>(UIMgr->GetWidget(UITags::HUD)))
		{
			HUD->HideWaveInfo();
		}
	}
}

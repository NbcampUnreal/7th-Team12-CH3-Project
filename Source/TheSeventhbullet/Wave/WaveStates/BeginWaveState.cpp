// Fill out your copyright notice in the Description page of Project Settings.


#include "BeginWaveState.h"

#include "Manager/UIManager.h"
#include "UI/UITags.h"
#include "UI/MainHUDWidget.h"

void UBeginWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Begin Wave"));
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;

	GM->SetupCurrentWaveData();
	DelayTimer = GM->GetWaveStartDelay();

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::TownHUD);

		if (GM->IsBossWave())
		{
			UIMgr->Open(UITags::BossHUD);
			UIMgr->Open(UITags::Crosshair);
		}
		else
		{
			UIMgr->Open(UITags::HUD);
			UIMgr->Open(UITags::Crosshair);

			if (UMainHUDWidget* HUD = Cast<UMainHUDWidget>(UIMgr->GetWidget(UITags::HUD)))
			{
				HUD->ShowWaveInfo(GM->GetCurrentWaveIndex() + 1, DelayTimer);
			}
		}
	}
}

void UBeginWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DelayTimer -= DeltaTime;

	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		if (UMainHUDWidget* HUD = Cast<UMainHUDWidget>(UIMgr->GetWidget(UITags::HUD)))
		{
			HUD->UpdateWaveTimer(FMath::Max(DelayTimer, 0.f));
		}
	}

	if (DelayTimer <= 0.0f)
	{
		ChangeState(EWaveState::Progress);
	}
}

void UBeginWaveState::Exit()
{
	Super::Exit();
}
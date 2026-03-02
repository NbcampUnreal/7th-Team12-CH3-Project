// Fill out your copyright notice in the Description page of Project Settings.


#include "IntermissionWaveState.h"

#include "Manager/UIManager.h"
#include "UI/UITags.h"
#include "UI/MainHUDWidget.h"

void UIntermissionWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Intermission Wave"));

	AMainGameMode* GM = GetGameMode();
	if (!GM) return;

	RestTimer = GM->GetIntermissionDuration();

	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		if (UMainHUDWidget* HUD = Cast<UMainHUDWidget>(UIMgr->GetWidget(UITags::HUD)))
		{
			HUD->ShowIntermission(RestTimer);
		}
	}
}

void UIntermissionWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RestTimer -= DeltaTime;

	if (UUIManager* UIMgr = UUIManager::Get(this))
	{
		if (UMainHUDWidget* HUD = Cast<UMainHUDWidget>(UIMgr->GetWidget(UITags::HUD)))
		{
			HUD->ShowIntermission(FMath::Max(RestTimer, 0.f));
		}
	}

	if (RestTimer <= 0.0f)
	{
		ChangeState(EWaveState::Begin);
	}
}

void UIntermissionWaveState::Exit()
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
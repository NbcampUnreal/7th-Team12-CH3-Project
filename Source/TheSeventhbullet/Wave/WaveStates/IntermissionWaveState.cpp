// Fill out your copyright notice in the Description page of Project Settings.


#include "IntermissionWaveState.h"

#include "Manager/UIManager.h"

void UIntermissionWaveState::Enter()
{
	Super::Enter();
	UE_LOG(LogTemp,Log,TEXT("Intermission Wave"));
	
	AMainGameMode* GM = GetGameMode();
	if (!GM) return;
	
	RestTimer = GM ? GM->GetIntermissionDuration() : 10.0f;
	
	//TODO 영섭 : 남은 시간 등등 휴식 카운트 출력
	// UUIManager* UIMgr = UUIManager::Get(this);
	// if (UIMgr)
	// {
	// 	UIMgr->ShowByTag(UITags::IntermissionPanel);
	// }
	
	
}

void UIntermissionWaveState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RestTimer -= DeltaTime;
	
	//TODO 영섭 : UI에 남은 시간 업데이트
	if (RestTimer <= 0.0f)
	{
		ChangeState(EWaveState::Begin);
	}
}

void UIntermissionWaveState::Exit()
{
	Super::Exit();

	//TODO 영섭 : UI 끄기
	// UUIManager* UIMgr = UUIManager::Get(this);
	// if (UIMgr)
	// {
	// 	UIMgr->HideByTag(UITags::IntermissionPanel);
	// }
}

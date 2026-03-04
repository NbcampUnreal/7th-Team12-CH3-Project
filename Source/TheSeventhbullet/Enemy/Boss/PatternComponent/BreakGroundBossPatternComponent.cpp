// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakGroundBossPatternComponent.h"

#include "BrainComponent.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/UIManager.h"
#include "System/MainGameMode.h"
#include "System/GameInstance/MainGameInstance.h"


void UBreakGroundBossPatternComponent::BossMonsterPlayPattern()
{
	//2번째 페이즈 구현
	if (BossLevelSequence!=nullptr)
	{
		ALevelSequenceActor* OutActor;
		FMovieSceneSequencePlaybackSettings Settings;
		//각각 바인딩된 더미들이 이동한 위치가 최종 위치에 반영
		Settings.bRestoreState_DEPRECATED=false;

		BossSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), BossLevelSequence, Settings, OutActor);
		if (BossSequencePlayer)
		{
			//AI를 컷신 재생동안만 일시정지
			BossEnemy=Cast<AEnemyBase>(GetOwner());
			if (BossEnemy==nullptr||BossEnemy->GetController()==nullptr||BossEnemy->GetController()->FindComponentByClass<UBrainComponent>()==nullptr)
			{
				return;
			}
			BossBrainComponent=BossEnemy->GetController()->FindComponentByClass<UBrainComponent>();
			BossBrainComponent->StopLogic(FString("PlayingBossLevelSequence"));
			BossSequencePlayer->OnFinished.AddDynamic(this,&UBreakGroundBossPatternComponent::OnBossSequenceFinishedDelegate);
			
			//애니메이션도 정지
			BossEnemy->StopAnimMontage();
			
			//플레이어를 시퀀스의 더미에 바인딩
			CharacterBinding=BossLevelSequence->FindBindingByTag(FName("ChangePlayer"));
			if (CharacterActors.Num()==0)
			{
				CharacterActors.Add(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
			}
			if (CharacterBinding.IsValid())
			{
				OutActor->SetBinding(CharacterBinding,CharacterActors);
			}
			//보스를 시퀀스의 더미에 바인딩
			BossBinding=BossLevelSequence->FindBindingByTag(FName("ChangeBoss"));
			if (BossActors.Num()==0)
			{
				UGameplayStatics::GetAllActorsWithTag(GetWorld(),FName("Boss"),BossActors);
			}
			if (BossBinding.IsValid())
			{
				OutActor->SetBinding(BossBinding,BossActors);
			}
			
			//재생 전에 HUD 제거
			UUIManager* UIMgr = UUIManager::Get(this);
			if (UIMgr)
			{
				UIMgr->Close(UITags::BossHUD);
				UIMgr->Close(UITags::Crosshair);
			}

			//레벨 시퀀스 재생
			BossSequencePlayer->Play();
		}
	}
}

void UBreakGroundBossPatternComponent::BreakGround()
{
	if (GetWorld()==nullptr)
	{
		return;
	}
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),BP_AnchorField,AnchorFields);
	for (auto& Anchor : AnchorFields)
	{
		Anchor->Destroy();
	}
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),BP_MasterField,MasterFields);
	for (auto& MasterField : MasterFields)
	{
		UFunction* TriggerFunc = MasterField->FindFunction(FName("CE_Trigger"));
		if (TriggerFunc!=nullptr)
		{
			MasterField->ProcessEvent(TriggerFunc, nullptr);
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Cannot Find CE_Trigger"));
		}
	}
}

void UBreakGroundBossPatternComponent::OnBossSequenceFinishedDelegate()
{
	if (BossBrainComponent!=nullptr)
	{
		BossBrainComponent->StartLogic();
	}
	//BossCharacter에게 패턴이 끝났음을 콜백
	OnBossPatternEndSignature.Broadcast();
	//재생 후에 HUD 오픈
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Open(UITags::BossHUD);
		UIMgr->Open(UITags::Crosshair);
	}
			
	
	//패턴은 일회용
	DestroyComponent();
}



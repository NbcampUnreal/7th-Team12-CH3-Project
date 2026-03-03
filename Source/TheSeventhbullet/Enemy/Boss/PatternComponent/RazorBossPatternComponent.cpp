// Fill out your copyright notice in the Description page of Project Settings.


#include "RazorBossPatternComponent.h"

#include "BrainComponent.h"
#include "BreakGroundBossPatternComponent.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"


void URazorBossPatternComponent::BossMonsterPlayPattern()
{
	//레이저 패턴 구현
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
			BossSequencePlayer->OnFinished.AddDynamic(this,&URazorBossPatternComponent::OnBossSequenceFinishedDelegate);
			
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
			
			//레벨 시퀀스 재생
			BossSequencePlayer->Play();
		}
	}
}

void URazorBossPatternComponent::OnBossSequenceFinishedDelegate()
{
	if (BossBrainComponent!=nullptr)
	{
		BossBrainComponent->StartLogic();
	}
	
	//BossCharacter에게 패턴이 끝났음을 콜백
	OnBossPatternEndSignature.Broadcast();
	
	//TODO: 잡몹소환 호출
	
	
	
	//패턴은 일회용
	DestroyComponent();
}





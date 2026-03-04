// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadBossPatternComponent.h"

#include "BrainComponent.h"
#include "BreakGroundBossPatternComponent.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/Boss/BossEnemyActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/UIManager.h"
#include "System/MainGameMode.h"


void UDeadBossPatternComponent::BossMonsterPlayPattern()
{
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
			BossSequencePlayer->OnFinished.AddDynamic(this,&UDeadBossPatternComponent::OnBossSequenceFinishedDelegate);
			
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
				UIMgr->Close(UITags::HUD);
			}
			
			
			//레벨 시퀀스 재생
			BossSequencePlayer->Play();
		}
	}
}

void UDeadBossPatternComponent::OnBossSequenceFinishedDelegate()
{
	
	BossEnemy->ReturnToPool();
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),BreakBossMapClass,BreakBossMap);
	for (auto& Actor : BreakBossMap)
	{
		Actor->Destroy();
	}
	BreakBossMap.Empty();
	if (BreakBossMapClass!=nullptr)
	{
		AActor* BreakGround1=GetWorld()->SpawnActor<AActor>(BreakBossMapClass,FVector(-5.461139f,3263.802785f,-11959.829517f),FRotator(0,90.0,0));
		BreakGround1->SetActorScale3D(FVector(1,1.7f,1));
		AActor* BreakGround2=GetWorld()->SpawnActor<AActor>(BreakBossMapClass,FVector(-3211.517476f,-0.000013f,-11959.829517f),FRotator(0,0,0));
		BreakGround2->SetActorScale3D(FVector(1,1,1));
		AActor* BreakGround3=GetWorld()->SpawnActor<AActor>(BreakBossMapClass,FVector(-4.498048,-3234.76249,-11959.829517),FRotator(0,90.0,0));
		BreakGround3->SetActorScale3D(FVector(1,1.7f,1));
		AActor* BreakGround4=GetWorld()->SpawnActor<AActor>(BreakBossMapClass,FVector(3187.631406,0.000031,-11959.829517),FRotator(0,0,0));
		BreakGround4->SetActorScale3D(FVector(1,1,1));
		if (BreakGround1&&BreakGround2&&BreakGround3&&BreakGround4)
		{
			UE_LOG(LogTemp,Warning,TEXT("SpawnBreakGroundSucceess"));
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("SpawnBreakGroundFailed"));
		}
	}
	FLatentActionInfo UnLoadInfo;
	UGameplayStatics::UnloadStreamLevel(this,FName("L_Boss"), UnLoadInfo, false);
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->OnPlayerDead();
	}
	//패턴은 일회용
	DestroyComponent();
}

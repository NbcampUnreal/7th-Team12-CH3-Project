// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIControllerBase.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Prediction.h"
#include "Perception/AISense_Sight.h"
#include "WereWolf/WereWolfCharacter.h"


// Sets default values
AEnemyAIControllerBase::AEnemyAIControllerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EnemyBehaviorTree = nullptr;
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

// Called when the game starts or when spawned
void AEnemyAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEnemyAIControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyAIControllerBase::SetBT(UBehaviorTree* EnemyBT)
{
	
}

void AEnemyAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//bool bSuccess = RunBehaviorTree(EnemyBehaviorTree);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), bSuccess?TEXT("TRUE"):TEXT("FALSE"));
	TObjectPtr<AEnemyBase> Enemy = Cast<AEnemyBase>(InPawn);
	Enemy->OnCharacterHit.AddDynamic(this, &AEnemyAIControllerBase::HitEvent);
	Enemy->OnCharacterDead.AddDynamic(this, &AEnemyAIControllerBase::DeadEvent);
	Enemy->OnCharacterReset.AddDynamic(this, &AEnemyAIControllerBase::ResetEvent);
	Enemy->OnCharacterHeadHit.AddDynamic(this, &AEnemyAIControllerBase::HeadHitEvent);
	Enemy->OnCharacterSetAI.AddDynamic(this,&AEnemyAIControllerBase::SetAI);
}

void AEnemyAIControllerBase::HitEvent()
{
	//헤드샷시 2초 경직
	UE_LOG(LogTemp, Warning, TEXT("HeadHit"));
	if (BBComp==nullptr) return;
	BBComp->SetValueAsBool(bIsHitKey, true);
}

void AEnemyAIControllerBase::DeadEvent()
{
	//사망 애니메이션 처리
	if (BBComp==nullptr) return;
	BBComp->SetValueAsBool(bIsDeadKey, true);
	UE_LOG(LogTemp, Warning, TEXT("DeadEvent"));
}

void AEnemyAIControllerBase::ResetEvent()
{
	if (BBComp==nullptr) return;
	BBComp->InitializeBlackboard(*EnemyBehaviorTree->BlackboardAsset);
	//명시적 초기화
	BBComp->SetValueAsBool(bIsDeadKey,false);
	BBComp->SetValueAsBool(bIsHitKey,false);
	RunBehaviorTree(EnemyBehaviorTree);
}

void AEnemyAIControllerBase::HeadHitEvent()
{
	if (BBComp==nullptr) return;
	BBComp->SetValueAsBool(bIsHeadHitKey, true);
}

void AEnemyAIControllerBase::SetAI(UBehaviorTree* ParamBT, float AttackRadius)
{
	UE_LOG(LogTemp, Warning, TEXT("SetAI"));
	if (ParamBT == nullptr) return;
	EnemyBehaviorTree = ParamBT;
	RunBehaviorTree(EnemyBehaviorTree);
	BBComp = GetBlackboardComponent();
	
	if (BBComp&&GetPawn())
	{
		BBComp->SetValueAsObject(TEXT("SelfActor"), GetPawn());
		BBComp->SetValueAsFloat(FName("AttackRadius"),AttackRadius);
	}
	if (AIPerceptionComp)
	{
		//공격 사거리가 시야보다 길다면, 시야를 사거리보다 크게 변경합니다.
		FAISenseID SightID = UAISense::GetSenseID<UAISense_Sight>();
		UAISenseConfig* Config = AIPerceptionComp->GetSenseConfig(SightID);
		UAISenseConfig_Sight* SightConfig = Cast<UAISenseConfig_Sight>(Config);

		if (SightConfig&&(SightConfig->SightRadius<AttackRadius))
		{
			SightConfig->SightRadius = AttackRadius+50.0f;
			SightConfig->LoseSightRadius =AttackRadius+150.0f;
			AIPerceptionComp->ConfigureSense(*SightConfig);
		}
		//퍼셉션 델리게이트 등록
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &AEnemyAIControllerBase::OnTargetPerceptionUpdated);
	}
}
//퍼셉션 인식 시 콜백 함수
void AEnemyAIControllerBase::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (BBComp == nullptr)
	{
		return;
	}
	//시각으로 본 경우
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			if (Actor->ActorHasTag(TEXT("Player")))
			{
				BBComp->SetValueAsObject(TEXT("TargetActor"), Actor);
			}
		}
		else
		{
			//Prediction 자극을 0.5초 뒤 예상 위치에 남김.
			UAISense_Prediction::RequestPawnPredictionEvent(GetPawn(),Actor,0.5f);
			BBComp->ClearValue(TEXT("TargetActor"));
			UE_LOG(LogTemp,Warning,TEXT("TargetLost"));
		}
		
		return;
	}
	//청각으로 들은 경우.
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		
		if (Stimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp,Warning,TEXT("SoundDetect"));
			GetBlackboardComponent()->SetValueAsVector(TEXT("DetectLocation"), Stimulus.StimulusLocation);
		}
		return;
	}
	//예측 마커가 찍힌 경우
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Prediction>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("DetectLocation"), Stimulus.StimulusLocation);
		}
		UE_LOG(LogTemp,Warning,TEXT("Prediction %s"), *Stimulus.StimulusLocation.ToString());
		return;
	}
	

	
	
}

//발견한 액터가 팀인지, 적인지, 중립인지 태그로 체크
ETeamAttitude::Type AEnemyAIControllerBase::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<APawn>(&Other);
	const ACharacter* OtherCharacter = Cast<ACharacter>(&Other);
	if (OtherCharacter==nullptr)
	{
		return ETeamAttitude::Neutral;
	}
	
	if (Other.ActorHasTag(TEXT("Player")))
	{
		return ETeamAttitude::Hostile;
	}
	else if (Other.ActorHasTag(TEXT("Enemy")))
	{
		return ETeamAttitude::Friendly;
	}
	return ETeamAttitude::Neutral;
}


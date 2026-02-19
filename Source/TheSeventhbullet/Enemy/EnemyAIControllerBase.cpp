// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIControllerBase.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WereWolf/WereWolfCharacter.h"


// Sets default values
AEnemyAIControllerBase::AEnemyAIControllerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EnemyBehaviorTree = nullptr;
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
	
}


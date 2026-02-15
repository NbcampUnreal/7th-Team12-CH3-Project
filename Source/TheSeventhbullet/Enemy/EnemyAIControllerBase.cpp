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

void AEnemyAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (EnemyBehaviorTree)
	{
		bool bSuccess = RunBehaviorTree(EnemyBehaviorTree);
		UE_LOG(LogTemp, Warning, TEXT("%s"), bSuccess?TEXT("TRUE"):TEXT("FALSE"));


		TObjectPtr<AEnemyBase> Enemy = Cast<AEnemyBase>(InPawn);
		Enemy->OnCharacterHit.AddDynamic(this, &AEnemyAIControllerBase::HitEvent);
		Enemy->OnCharacterDead.AddDynamic(this,&AEnemyAIControllerBase::DeadEvent);
		BBComp = GetBlackboardComponent();
		BBComp->SetValueAsObject(TEXT("SelfActor"),InPawn);
	}
}

void AEnemyAIControllerBase::HitEvent()
{
	//헤드샷시 2초 경직
	BBComp->SetValueAsBool(TEXT("bIsHit"), true);
}

void AEnemyAIControllerBase::DeadEvent()
{
	//사망 애니메이션 처리
	BBComp->SetValueAsBool(TEXT("bIsDead"), true);
	UE_LOG(LogTemp,Warning,TEXT("DeadEvent"));
}

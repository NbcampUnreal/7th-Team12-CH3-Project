// Fill out your copyright notice in the Description page of Project Settings.


#include "WereWolfCharacter.h"
#include "WereWolfAIController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AWereWolfCharacter::AWereWolfCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AWereWolfAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AWereWolfCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWereWolfCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

//블루프린트 디버깅용 입력 받기
// Called to bind functionality to input
void AWereWolfCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}







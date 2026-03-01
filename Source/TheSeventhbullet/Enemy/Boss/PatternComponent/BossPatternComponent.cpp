// Fill out your copyright notice in the Description page of Project Settings.


#include "BossPatternComponent.h"

#include "System/MainGameMode.h"

// Sets default values for this component's properties
UBossPatternComponent::UBossPatternComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UBossPatternComponent::BossMonsterTriggerPattern()
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;
	GM->TriggerBossPatternSpawn(1);
}


// Called when the game starts
void UBossPatternComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBossPatternComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamTrigger.h"

#include "Character/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/MainGameMode.h"

ALevelStreamTrigger::ALevelStreamTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;
	
	OverlapVolume->SetCollisionProfileName(TEXT("Trigger"));
	OverlapVolume->SetBoxExtent(FVector(100.f,100.f,100.f));
}

void ALevelStreamTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (OverlapVolume)
	{
		OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamTrigger::OnOverlapBegin);
	}
}

void ALevelStreamTrigger::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		FLatentActionInfo LatentInfo;//비동기 작업의 진행 상태를 추적하기 위해 필요한 구조체
		LatentInfo.CallbackTarget = this;
		
		FLatentActionInfo UnloadLatentInfo;
		UnloadLatentInfo.CallbackTarget = this;
		UnloadLatentInfo.ExecutionFunction = NAME_None;
		UnloadLatentInfo.Linkage = 1;
		UnloadLatentInfo.UUID = GetUniqueID() + 1;
		
		if (LevelToLoad != NAME_None)
		{
			UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, false, LatentInfo);
		}
		if (LevelToUnload != NAME_None)
		{
			UGameplayStatics::UnloadStreamLevel(this, LevelToUnload, UnloadLatentInfo, false);
		}
	}
}

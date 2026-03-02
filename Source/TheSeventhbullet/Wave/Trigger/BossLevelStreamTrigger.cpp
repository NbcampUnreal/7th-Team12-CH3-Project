#include "BossLevelStreamTrigger.h"

#include "System/GameInstance/MainGameInstance.h"

ABossLevelStreamTrigger::ABossLevelStreamTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABossLevelStreamTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABossLevelStreamTrigger::ActivateTrigger()
{
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (!GI) return;
	
	GI->RequestBossStage(BossRequestID);
}


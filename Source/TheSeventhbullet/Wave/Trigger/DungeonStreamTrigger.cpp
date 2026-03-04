#include "DungeonStreamTrigger.h"

#include "Manager/SoundManager.h"
#include "System/MainGameMode.h"
#include "System/MonsterManagerSubSystem.h"

void ADungeonStreamTrigger::ActivateTrigger()
{
	Super::ActivateTrigger();
}

bool ADungeonStreamTrigger::CanActivate() const
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM || !GM->HasActiveRequest()) return false;

	// 보스 의뢰(ID 100)일 때는 일반 던전 입장 불가
	constexpr int32 BossRequestID = 100;
	return GM->GetCurrentRequestID() != BossRequestID;
}

void ADungeonStreamTrigger::HandleLevelLoaded()
{
	UMonsterManagerSubSystem* SubSystem = UMonsterManagerSubSystem::Get(this);
	if (!SubSystem) return;
	SubSystem->CacheSpawners();

	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;
	GM->PrepareStageAndPreLoad();
	
	USoundManager* SoundMgr = USoundManager::Get(this);
	if (SoundMgr)
	{
		SoundMgr->PlayBGM(TEXT("BattleBGM"), 2.0f, 0.7f);
	}
}

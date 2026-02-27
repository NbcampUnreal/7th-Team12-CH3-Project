#include "DungeonStreamTrigger.h"

#include "System/MainGameMode.h"
#include "System/MonsterManagerSubSystem.h"

bool ADungeonStreamTrigger::CanActivate() const
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	return GM && GM->HasActiveRequest();
}

void ADungeonStreamTrigger::HandleLevelLoaded()
{
	UMonsterManagerSubSystem* SubSystem = UMonsterManagerSubSystem::Get(this);
	if (!SubSystem) return;
	SubSystem->CacheSpawners();

	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;
	GM->PrepareStageAndPreLoad();
}

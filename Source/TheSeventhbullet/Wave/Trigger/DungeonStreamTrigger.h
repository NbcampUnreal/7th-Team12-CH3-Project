#pragma once

#include "CoreMinimal.h"
#include "LevelStreamTrigger.h"
#include "DungeonStreamTrigger.generated.h"

/**
 * 던전 전용 스트리밍 트리거
 *
 * 의뢰를 수락한 상태(HasActiveRequest)에서만 진입 가능.
 * 레벨 로드 완료 시 몬스터 풀 초기화 + 웨이브 시작.
 */
UCLASS()
class THESEVENTHBULLET_API ADungeonStreamTrigger : public ALevelStreamTrigger
{
	GENERATED_BODY()

protected:
	virtual bool CanActivate() const override;
	virtual void HandleLevelLoaded() override;
};

#pragma once

#include "CoreMinimal.h"
#include "LevelStreamTrigger.h"
#include "BossLevelStreamTrigger.generated.h"

UCLASS()
class THESEVENTHBULLET_API ABossLevelStreamTrigger : public ALevelStreamTrigger
{
	GENERATED_BODY()

public:
	ABossLevelStreamTrigger();
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Boss")
	int32 BossRequestID = 100;
protected:
	virtual void BeginPlay() override;
	virtual void ActivateTrigger() override;
};

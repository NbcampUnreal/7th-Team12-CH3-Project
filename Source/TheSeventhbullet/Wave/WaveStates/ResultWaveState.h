#pragma once

#include "CoreMinimal.h"
#include "BaseWaveState.h"
#include "ResultWaveState.generated.h"

UCLASS()
class THESEVENTHBULLET_API UResultWaveState : public UBaseWaveState
{
	GENERATED_BODY()
public:
	virtual void Enter() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Exit() override;
private:
	bool bWaitingForInput = false;
};

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomLocation.generated.h"


UCLASS()
class THESEVENTHBULLET_API UBTTask_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindRandomLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float SearchRadius = 1000.0f;
};

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayAnimMontageBase.generated.h"

/**
 * 애니메이션을 재생하고 재생 종료 성공시 Succeeded를 반환합니다.
 */
UCLASS()
class THESEVENTHBULLET_API UBTTask_PlayAnimMontageBase : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayAnimMontageBase();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	//옵저버 어봇: 중단될 시 호출되는 함수. 
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	// 성공/실패/중단, 아무튼 무관하게 종료될 시
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "AnimationMontage")
	FName AnimMontageName;
	
	
};

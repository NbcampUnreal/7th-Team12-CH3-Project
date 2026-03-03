////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * CraftingActor - CraftingComponent를 가진 제작대 액터
 *
 * 플레이어가 가까이 가서 Interact하면 CraftingWidget을 열 수 있음.
 * CraftingComponent를 소유하여 소울젬 제작을 수행.
 * ABaseInteractionActor를 상속받아 Overlap/프롬프트 처리를 부모에게 위임.
 *
 * [사용법]
 *   1. 레벨에 CraftingActor 배치
 *   2. 플레이어가 트리거 범위 진입 → "F 상호작용" 프롬프트 표시
 *   3. F키 입력 → CraftingWidget이 열림
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionActor.h"
#include "CraftingActor.generated.h"

class UCraftingComponent;

UCLASS()
class THESEVENTHBULLET_API ACraftingActor : public ABaseInteractionActor
{
	GENERATED_BODY()

public:
	ACraftingActor();

	virtual void Interact(AActor* Interactor) override;

	UFUNCTION(BlueprintPure, Category = "Crafting")
	UCraftingComponent* GetCraftingComponent() const { return CraftingComp; }

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Crafting")
	TObjectPtr<UCraftingComponent> CraftingComp;
};

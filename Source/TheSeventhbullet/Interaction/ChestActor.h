////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * ChestActor - 인벤토리를 가진 창고 액터
 *
 * 플레이어가 가까이 가서 Interact하면 StorageWidget을 열 수 있음.
 * InventoryComponent를 소유하여 아이템을 보관.
 * ABaseInteractionActor를 상속받아 Overlap/프롬프트 처리를 부모에게 위임.
 *
 * [사용법]
 *   1. 레벨에 ChestActor 배치
 *   2. 플레이어가 트리거 범위 진입 → "F 상호작용" 프롬프트 표시
 *   3. F키 입력 → StorageWidget이 Push됨 (인벤토리 + 창고 나란히 표시)
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionActor.h"
#include "ChestActor.generated.h"

class UInventoryComponent;

UCLASS()
class THESEVENTHBULLET_API AChestActor : public ABaseInteractionActor
{
	GENERATED_BODY()

public:
	AChestActor();

	virtual void Interact(AActor* Interactor) override;

	UFUNCTION(BlueprintPure, Category = "Chest")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComp; }

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<UInventoryComponent> InventoryComp;
};

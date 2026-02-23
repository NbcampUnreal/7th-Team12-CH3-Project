////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * ChestActor - 인벤토리를 가진 창고 액터
 *
 * 플레이어가 가까이 가서 Interact하면 StorageWidget을 열 수 있음.
 * InventoryComponent를 소유하여 아이템을 보관.
 *
 * [사용법]
 *   1. 레벨에 ChestActor 배치
 *   2. 플레이어가 트리거 범위 진입 → "F 상자 열기" 프롬프트 표시
 *   3. F키 입력 → StorageWidget이 Push됨 (인벤토리 + 창고 나란히 표시)
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChestActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UInventoryComponent;

UCLASS()
class THESEVENTHBULLET_API AChestActor : public AActor
{
	GENERATED_BODY()

public:
	AChestActor();

	void Interact(AActor* Caller);

	UFUNCTION(BlueprintPure, Category = "Chest")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComp; }

	bool IsPlayerInRange() const { return bPlayerInRange; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Chest")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UPROPERTY(VisibleAnywhere, Category = "Chest")
	TObjectPtr<UStaticMeshComponent> ChestMesh;

	UPROPERTY(VisibleAnywhere, Category = "Chest")
	TObjectPtr<UWidgetComponent> InteractionWidget;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<UInventoryComponent> InventoryComp;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|UI")
	TSubclassOf<UUserWidget> PromptWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|UI")
	FVector PromptOffset = FVector(0.0f, 0.0f, 100.0f);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bPlayerInRange = false;
};

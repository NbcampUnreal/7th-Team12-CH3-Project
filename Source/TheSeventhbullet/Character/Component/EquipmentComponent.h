#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulGem/SoulGemInstance.h"
#include "EquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 장착된 소울젬이 변경되면 호출
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentChanged OnEquipmentChanged;
	
	// 장착된 소울젬 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<FSoulGemInstance> EquippedSoulGems;
	
	// 소울젬 장착
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipSoulGem(const FSoulGemInstance& SoulGem, int32 SlotIndex);
	// 스탯 모디파이어 수집
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void CollectStatusModifiers(TArray<FStatusModifier>& Mod) const;
	
	FCharacterStat GetTotalGemStats() const;
};

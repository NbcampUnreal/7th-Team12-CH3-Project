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
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentChanged OnEquipmentChanged;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<FSoulGemInstance> EquippedSoulGems;
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipSoulGem(const FSoulGemInstance& SoulGem, int32 SlotIndex);
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void CollectStatusModifiers(TArray<FStatusModifier>& Mod) const;
};

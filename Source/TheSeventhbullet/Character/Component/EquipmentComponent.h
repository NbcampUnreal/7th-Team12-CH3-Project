#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulGem/SoulGemInstance.h"
#include "EquipmentComponent.generated.h"

class UWeaponDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGemEquipmentChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipmentChanged, UWeaponDataAsset*, WeaponData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 장착된 소울젬 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<FSoulGemInstance> EquippedSoulGems;
	// 장착된 무기 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UWeaponDataAsset> CurrentWeapon = nullptr;
	
	// 소울젬 장착
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipSoulGem(const FSoulGemInstance& SoulGem, int32 SlotIndex);
	// 스탯 모디파이어 수집
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void CollectStatusModifiers(TArray<FStatusModifier>& Mod) const;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipWeaponData(UWeaponDataAsset* Weapon);
	
	FCharacterStat GetTotalGemStats() const;
	
public:
	// 장착된 소울젬이 변경
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnGemEquipmentChanged OnGemEquipmentChanged;
	// 장착된 무기가 변경
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnWeaponEquipmentChanged OnWeaponEquipmentChanged;
};

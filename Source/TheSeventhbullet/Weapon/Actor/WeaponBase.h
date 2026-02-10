#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheSeventhbullet/Weapon/Interface/Equipable.h"
#include "TheSeventhbullet/Weapon/Interface/Fireable.h"
#include "WeaponBase.generated.h"

class USphereComponent;

UCLASS()
class THESEVENTHBULLET_API AWeaponBase
	: public AActor,
	  public IFireable,
	  public IEquipable
{
	GENERATED_BODY()

public:
	AWeaponBase();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
	TObjectPtr<USceneComponent> Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	virtual void Equip(TObjectPtr<AActor> NewOwner) override;
	virtual void UnEquip() override;
	
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void Reload() override;
	
	virtual void Fire();
	void ConsumeAmmo();

protected:
	int32 MaxAmmo = 6;
	int32 CurrentAmmo = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AActor> WeaponOwner;
	
};

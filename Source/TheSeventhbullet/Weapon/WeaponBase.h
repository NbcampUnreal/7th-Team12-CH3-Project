#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Fireable.h"
#include "WeaponBase.generated.h"

UCLASS()
class THESEVENTHBULLET_API AWeaponBase : public AActor, public IFireable
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
	TObjectPtr<USceneComponent> Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void Fire() override;
	virtual void Reload() override;	
};

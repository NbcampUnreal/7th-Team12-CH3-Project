#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSkill.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class THESEVENTHBULLET_API APlayerSkill : public AActor
{
	GENERATED_BODY()

public:
	APlayerSkill();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Component")
	USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Component")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Component")
	UProjectileMovementComponent* ProjectileMovement;
};

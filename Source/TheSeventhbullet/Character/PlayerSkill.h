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
#pragma region Components
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Component")
	USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Component")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Component")
	UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	USoundBase* ExplodeSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	UParticleSystem* ExplodeParticle;
	
#pragma endregion
	
#pragma region Explosion
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Explode")
	float ExplodeRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Explode")
	float DamageAmount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Explode")
	float KnockBackAmount;
	
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
		);
	
	void Explode();
	
#pragma endregion
};

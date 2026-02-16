// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterEventSignnature);


UCLASS()
class THESEVENTHBULLET_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnCharacterHit;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnCharacterDead;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable,Category="Enemy|Status")
	void SetupEnemy(float InMaxHealth=100.0f, float InArmorPoint=0.0f,float InAttackPoint=10.0f, float InKnockbackStrengh=200.0f);
	
	float GetAttackPoint();
	



protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float NowHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float MaxHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float ArmorPoint;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float AttackPoint;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float KnockbackStrengh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Hit")
	TObjectPtr<UAnimMontage> HitAnimMontage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Hit")
	UParticleSystem* HitParticle;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Hit")
    UParticleSystem* HeadShotParticle;
	
	
	
	UFUNCTION(BlueprintCallable)
	virtual void EnemyTakePointDamage(
	AActor* DamagedActor, 
	float Damage, 
	class AController* InstigatedBy, 
	FVector HitLocation, 
	class UPrimitiveComponent* FHitComponent, 
	FName BoneName, 
	FVector ShotFromDirection, 
	const class UDamageType* DamageType, 
	AActor* DamageCauser
	);
	
	void SetHealth(float NewHealth);
	void DisplayParticle(FVector HitLocation, UParticleSystem* InParticle);
	void ReturnToPool();

private:
	bool bIsDead;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy/EnemyBase.h"
#include "BossEnemyActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UBossEnemyActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnBossPatternEnd;
    
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnBossCanceled;
    
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnBossFirstHalfHealth;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnBossFinalPhase;
	
	UFUNCTION(BlueprintCallable)
	void SetBoss();

	void PlayPattern(FString PatternName);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class AEnemyBase* OwnerEnemy;
	
	bool bIsFirstHalfHealth = false;
	bool bIsFirstFinalPhase = false;
	
	UPROPERTY(VisibleAnywhere, Category = "Boss")
	TMap<FString, TObjectPtr<UBossPatternComponentInterface>> SpawnedBossPattern;
	
	UFUNCTION()
	void OwnerTakeDamage(
		AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser);

	void SetHealth(float NewHealth);


	
	UFUNCTION()
	void EndPattern();
};

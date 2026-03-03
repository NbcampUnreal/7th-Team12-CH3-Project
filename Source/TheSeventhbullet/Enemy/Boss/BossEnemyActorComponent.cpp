// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemyActorComponent.h"

#include "BossAIController.h"
#include "Character/DamageType/PlayerSkillDamageType.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/EnemyDataAsset.h"
#include "PatternComponent/BossPatternComponentInterface.h"


// Called when the game starts
void UBossEnemyActorComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerEnemy = Cast<AEnemyBase>(GetOwner());
	if (OwnerEnemy)
	{
		OwnerEnemy->OnTakeAnyDamage.AddDynamic(this, &UBossEnemyActorComponent::OwnerTakeDamage);
	}
	
}

void UBossEnemyActorComponent::OwnerTakeDamage(
	AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp,Warning,TEXT("OwnerTakeDamage"));
	if (!OwnerEnemy)
	{
		return;
	}
	if (DamageType && DamageType->IsA(UPlayerSkillDamageType::StaticClass()))
	{
		OnBossCanceled.Broadcast();
		UE_LOG(LogTemp,Warning,TEXT("PlayerSkillHit"));
	}
	
	SetHealth(OwnerEnemy->GetHealth());
	
}

void UBossEnemyActorComponent::SetHealth(float NewHealth)
{
	NewHealth = FMath::Clamp(NewHealth, 0.0f, 9999999.0f);
	UE_LOG(LogTemp,Warning,TEXT("%f"),NewHealth);
	
	if (FMath::IsNearlyZero(NewHealth))
	{
		if (!bIsFirstFinalPhase)
		{
			UE_LOG(LogTemp,Warning,TEXT("FinalPhase"));
			bIsFirstFinalPhase=true;
			OnBossFinalPhase.Broadcast();
		}
	}
	
	
	UE_LOG(LogTemp,Warning,TEXT("HP : %f"),NewHealth);
	if (!bIsFirstHalfHealth&&NewHealth<=OwnerEnemy->GetMaxHealth()/2.0f)
	{
		bIsFirstHalfHealth=true;
		OnBossFirstHalfHealth.Broadcast();
	}
}

void UBossEnemyActorComponent::EndPattern()
{
	OnBossPatternEnd.Broadcast();
}

void UBossEnemyActorComponent::SetBoss()
{
	if (!OwnerEnemy) return;
	
	if (UCapsuleComponent* CapsuleComponent = OwnerEnemy->GetCapsuleComponent())
	{
		CapsuleComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
		CapsuleComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
	OwnerEnemy->Tags.Add(FName("Boss"));
	OwnerEnemy->SetActorScale3D(FVector(4.0f));

	//컴포넌트 추가
	if (OwnerEnemy->EnemyData==nullptr)
	{
		return;
	}
	for (auto& BossPattern : OwnerEnemy->EnemyData->PatternClasses)
	{
		if (BossPattern.Value==nullptr)
		{
			continue;
		}
		UBossPatternComponentInterface* PatternComp=NewObject<UBossPatternComponentInterface>(this, BossPattern.Value);
		if (PatternComp==nullptr)
		{
			return;
		}
		PatternComp->RegisterComponent();
		SpawnedBossPattern.Add(BossPattern.Key, PatternComp);
		OwnerEnemy->AddInstanceComponent(PatternComp);
		PatternComp->OnBossPatternEndSignature.AddDynamic(this, &UBossEnemyActorComponent::EndPattern);
	}
	
	if (OwnerEnemy->GetController()!=nullptr)
	{
		ABossAIController* BossAIController=Cast<ABossAIController>(OwnerEnemy->GetController());
		if (BossAIController!=nullptr)
		{
			OnBossPatternEnd.AddDynamic(BossAIController,&ABossAIController::BossPatternEndEvent);
			OnBossCanceled.AddDynamic(BossAIController,&ABossAIController::BossCancelEvent);
			OnBossFirstHalfHealth.AddDynamic(BossAIController,&ABossAIController::BossHalfHealthEvent);
			OnBossFinalPhase.AddDynamic(BossAIController,&ABossAIController::BossFinalPhaseEvent);
		}
	}
	
	
}

void UBossEnemyActorComponent::PlayPattern(FString PatternName)
{
	UE_LOG(LogTemp,Warning,TEXT("PlayPattern"));
	if (SpawnedBossPattern.Find(PatternName)==nullptr||SpawnedBossPattern[PatternName]==nullptr)
	{
		return;
	}
	SpawnedBossPattern[PatternName]->BossMonsterPlayPattern();
}



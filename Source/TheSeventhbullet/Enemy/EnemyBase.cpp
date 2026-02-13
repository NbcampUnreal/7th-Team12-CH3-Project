// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MaxHealth = 100.0f;
	NowHealth = MaxHealth;
	ArmorPoint = 0.f;
	AttackPoint = 10.0f;
	KnockbackStrengh = 200.0f;
	
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AEnemyBase::OnTakePointDamage.AddDynamic(this, &AEnemyBase::EnemyTakePointDamage);
	
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyBase::SetupEnemy(float InMaxHealth, float InArmorPoint, float InAttackPoint, float InKnockbackStrengh)
{
	MaxHealth=InMaxHealth;
	ArmorPoint=InArmorPoint;
	AttackPoint=InAttackPoint;
	KnockbackStrengh=InKnockbackStrengh;
}


void AEnemyBase::EnemyTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy,
                                      FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName,
                                      FVector ShotFromDirection,
                                      const class UDamageType* DamageType, AActor* DamageCauser)
{
	//헤드샷 여부
	bool bIsHeadShot = false;
	if (BoneName == TEXT("head"))
	{
		bIsHeadShot = true;
	}
	
	//데미지 처리- 헤드샷 1.5배 데미지
	SetHealth(NowHealth + ArmorPoint - (bIsHeadShot ? Damage * 1.5f : Damage));
	UE_LOG(LogTemp, Warning, TEXT("LineTraceHit, %s, %f"), *BoneName.ToString(), NowHealth);
	
	//헤드샷을 맞았을 경우
	if (bIsHeadShot)
	{
		//HIT상태 전달을 위한 델리게이트
		OnCharacterHit.Broadcast();
		
		//넉백 구현
		FVector LaunchVelocity = ShotFromDirection * KnockbackStrengh;
		LaunchCharacter(bIsHeadShot ? LaunchVelocity : FVector::ZeroVector, true, true);
		
		
		
		if (HeadShotParticle)
		{
			DisplayParticle(HitLocation, HeadShotParticle);
		}
	}
	
	//몸통에 맞았을 경우
	else
	{
		if (HitParticle)
		{
			DisplayParticle(HitLocation, HitParticle);
		}
	}
	

	
	
	
	



	
}

void AEnemyBase::SetHealth(float NewHealth)
{
	NowHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}

void AEnemyBase::DisplayParticle(FVector HitLocation, UParticleSystem* InParticle)
{
	TObjectPtr<UParticleSystemComponent> Particle = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		InParticle,
		HitLocation,
		GetActorRotation(),
		true
	);
	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;

		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (UParticleSystemComponent* P = WeakParticle.Get())
				{
					P->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}





// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "AIController.h"
#include "EnemyAIControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "AnimInstance/EnemyAnimInstance.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/EnemyDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AEnemyAIControllerBase::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MaxHealth = 100.0f;
	NowHealth = MaxHealth;
	ArmorPoint = 0.f;
	AttackPoint = 10.0f;
	KnockbackStrengh = 200.0f;
	bIsDead = false;
	bIsLongRange=false;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AEnemyBase::OnTakePointDamage.AddDynamic(this, &AEnemyBase::EnemyTakePointDamage);
	Tags.Add(FName("Enemy"));
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

//TODO : 프라이머리 데이터 에셋으로 초기화
void AEnemyBase::SetupEnemy(UEnemyDataAsset* LoadedData)
{
	EnemyData=LoadedData;
	MaxHealth=EnemyData->MaxHealth;
	NowHealth=MaxHealth;
	ArmorPoint=EnemyData->ArmorPoint;
	AttackPoint=EnemyData->AttackPoint;
	bIsLongRange=EnemyData->bIsLongRange;
	HitParticle=EnemyData->HitParticle.Get();
	HeadShotParticle=EnemyData->HeadShotParticle.Get();
	USkeletalMeshComponent* EnemyMeshComp=this->GetMesh();
	
	if (EnemyMeshComp)
	{
		this->GetMesh()->SetSkeletalMeshAsset(EnemyData->SkeletalMesh.Get());
		this->GetMesh()->SetAnimInstanceClass(EnemyData->EnemyAnimationBlueprint.Get());
		this->GetMesh()->SetRelativeLocation(FVector(0,0,-90.f));
		this->GetMesh()->SetRelativeRotation(FRotator(0,-90,0));
		this->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		UAnimInstance* AnimInstance=this->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (UEnemyAnimInstance* EnemyAnimInstance=Cast<UEnemyAnimInstance>(AnimInstance))
			{
				EnemyAnimInstance->SetEnemyBlendSpace(EnemyData->EnemyBlendSpace.Get());
			}
		}
	}
	AEnemyAIControllerBase* EnemyAIControllerBase=Cast<AEnemyAIControllerBase>(this->GetController());
	if (EnemyAIControllerBase)
	{
		EnemyBehaviorTree=EnemyData->EnemyBT.Get();
		EnemyAIControllerBase->SetBT(EnemyBehaviorTree);
	}
	
}




float AEnemyBase::GetAttackPoint()
{
	return AttackPoint;
}

void AEnemyBase::ResetEnemy()
{
	//HP를 최대치로 채운다.
	NowHealth=MaxHealth;
	//BT를 초기화 후 재가동하기 위한 델리게이트
	OnCharacterReset.Broadcast();
}

UAnimMontage* AEnemyBase::ReturnthisMontage(FName AMName)
{
	if (EnemyData==nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("%s"),*AMName.ToString());
		return nullptr;
	}
	return EnemyData->AnimMontages[AMName].Get();
}

UAnimMontage* AEnemyBase::ReturnthisProjectileMontage()
{
	if (EnemyData==nullptr) return nullptr;
	return EnemyData->ProjectileAnimMontage.Get();
}

EMonsterType AEnemyBase::GetMonsterType()
{
	return EnemyMonsterType;
}

void AEnemyBase::SetMonsterType(EMonsterType InEnemyMonsterType)
{
	EnemyMonsterType=InEnemyMonsterType;
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

	// 적 캐릭터가 처음 죽었을 경우
	if (!bIsDead && FMath::IsNearlyZero(NowHealth))
	{
		//사망 처리
		bIsDead = true;
		
		//BT에 정보 전달
		OnCharacterDead.Broadcast();
		
		
		//5초 후 오브젝트 풀로 돌아간다.
		FTimerHandle ReturnToPoolTimer;
		GetWorld()->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &AEnemyBase::ReturnToPool, 5.0f, false);
	}


	//헤드샷을 맞았을 경우
	if (bIsHeadShot)
	{
		//HIT상태 전달을 위한 델리게이트
		OnCharacterHit.Broadcast();

		//넉백 구현 - 미적용
		//FVector LaunchVelocity = ShotFromDirection * KnockbackStrengh;
		//LaunchCharacter(bIsHeadShot ? LaunchVelocity : FVector::ZeroVector, true, true);


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

void AEnemyBase::ReturnToPool()
{
	//TODO : 오브젝트 풀로 리턴하는 함수를 호출
	UE_LOG(LogTemp,Warning,TEXT("ReturnToPool"));
	Destroy();
}

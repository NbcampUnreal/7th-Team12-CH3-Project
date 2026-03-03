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
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DamageType/PlayerSkillDamageType.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/EnemyDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/Material.h"
#include "Data/TableRowTypes.h"
#include "System/MonsterManagerSubSystem.h"
#include "Data/TableRowTypes.h"
#include "Engine/DamageEvents.h"
#include "Projectile/ProjectileStat.h"
#include "System/MainGameMode.h"
#include "System/GameInstance/MainGameInstance.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
	//별도로 데이터를 주입하지 않은 캐릭터의 기본 스테이터스
	MaxHealth = 100.0f;
	NowHealth = MaxHealth;
	ArmorPoint = 0.f;
	AttackPoint = 10.0f;
	KnockbackStrengh = 200.0f;
	bIsDead = false;
	
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

// (데이터 드리븐)프라이머리 데이터 에셋으로 초기화
void AEnemyBase::SetupEnemy(UEnemyDataAsset* LoadedData)
{
	EnemyData=LoadedData;
	MaxHealth=EnemyData->MaxHealth;
	ArmorPoint=EnemyData->ArmorPoint;
	AttackPoint=EnemyData->AttackPoint;
	AttackRadius=EnemyData->AttackRadius;
	GetCharacterMovement()->MaxWalkSpeed=EnemyData->Speed;
	HitParticle=EnemyData->HitParticle.Get();
	HeadShotParticle=EnemyData->HeadShotParticle.Get();
	USkeletalMeshComponent* EnemyMeshComp=this->GetMesh();
	
	//일차 기반으로 공격력 상승. 상승폭은 x^2 % 
	GI=UMainGameInstance::Get(this);
	CurrentDay=GI->CurrentDay;
	MaxHealth=MaxHealth*((CurrentDay*CurrentDay)/100.0f)+MaxHealth;
	NowHealth=MaxHealth;
	ArmorPoint=ArmorPoint*((CurrentDay*CurrentDay)/100.0f)+ArmorPoint;
	AttackPoint=AttackPoint*((CurrentDay*CurrentDay)/100.0f)+AttackPoint;
	
	//Mesh관련 처리
	if (EnemyMeshComp)
	{
		this->GetMesh()->SetSkeletalMeshAsset(EnemyData->SkeletalMesh.Get());
		this->GetMesh()->SetAnimInstanceClass(EnemyData->EnemyAnimationBlueprint.Get());
		this->GetMesh()->SetRelativeLocation(FVector(0,0,-90.f));
		this->GetMesh()->SetRelativeRotation(FRotator(0,-90,0));
		this->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		this->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		this->GetMesh()->SetOverlayMaterial(EnemyData->EnemyMaterial.Get());
		
		UAnimInstance* AnimInstance=this->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (UEnemyAnimInstance* EnemyAnimInstance=Cast<UEnemyAnimInstance>(AnimInstance))
			{
				EnemyAnimInstance->SetEnemyBlendSpace(EnemyData->EnemyBlendSpace.Get());
			}
		}
	}
	if (EnemyData->AIControllerClass.Get()!=nullptr)
	{
		AIControllerClass = EnemyData->AIControllerClass.Get();
		AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
		if (GetController() == nullptr)
		{
			SpawnDefaultController();
		}
		//BehaviorTree 및 AttackRadius, bIsLongRange, Speed BB키 세팅
		OnCharacterSetAI.Broadcast(EnemyData->EnemyBT.Get(),AttackRadius,EnemyData->bIsLongRange,EnemyData->Speed,EnemyData->StrafeSpeed,EnemyData->EnemyAttackDelay);
		
	}
	PStatus=MakeShared<FProjectileStatus>();
	PStatus->StaticMesh=EnemyData->ProjectileStaticMesh.Get();
	PStatus->Speed=EnemyData->ProjectileSpeed;
	PStatus->bIsHoming=EnemyData->bIsHoming;
	PStatus->Material=EnemyData->EnemyMaterial.Get();
	
}



float AEnemyBase::GetAttackPoint()
{
	return AttackPoint;
}


TSharedPtr<FProjectileStatus> AEnemyBase::GetProjectileStatus()
{
	return PStatus;	
}

void AEnemyBase::ResetEnemy()
{
	//HP를 최대치로 채운다.
	NowHealth=MaxHealth;
	//BT를 초기화 후 재가동하기 위한 델리게이트
	OnCharacterReset.Broadcast();
	
	//사망 상태를 다시 바꾼다.
	bIsDead=false;
	bDroppedItem=false;
	
	//일시정지되었던 애니메이션 재시작
	GetMesh()->bPauseAnims=false;
	GetMesh()->SetComponentTickEnabled(true);
	
	
}

UAnimMontage* AEnemyBase::ReturnthisMontage(FName AMName)
{
	if (EnemyData==nullptr)
	{
		return nullptr;
	}
	if (EnemyData->AnimMontages.Contains(AMName))
	{
		return EnemyData->AnimMontages[AMName].Get();
	}
	return nullptr;
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
	// 최종 데미지 : 데미지-방어력, 0보다 작아질 수는 없음.
	FinalDamage=(bIsHeadShot ? Damage * 1.5f : Damage)-ArmorPoint;
	if (FinalDamage<0)
	{
		FinalDamage=0;
	}
	SetHealth(NowHealth - FinalDamage);

	// 적 캐릭터가 처음 죽었을 경우
	if (!bIsDead && FMath::IsNearlyZero(NowHealth)&&(!this->Tags.Find("Boss")))
	{
		//사망 처리
		bIsDead = true;
		
		//BT에 정보 전달
		OnCharacterDead.Broadcast();
		
		DropItem();
		
		//5초 후 오브젝트 풀로 돌아간다.
		FTimerHandle ReturnToPoolTimer;
		GetWorld()->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &AEnemyBase::ReturnToPool, 5.0f, false);
	}


	//헤드샷을 맞았을 경우
	if (bIsHeadShot)
	{
		//HEADHIT상태 전달을 위한 델리게이트
		OnCharacterHeadHit.Broadcast();

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
		//HIT상태 전달을 위한 델리게이트
		OnCharacterHit.Broadcast();
		if (HitParticle)
		{
			DisplayParticle(HitLocation, HitParticle);
		}
	}
	
}

void AEnemyBase::DropItem()
{
	if (bDroppedItem) return;
	bDroppedItem = true;

	if (AMainGameMode* GM = AMainGameMode::Get(this))
	{
		GM->ItemDropFromMonster(EnemyMonsterType);
	}
}
float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	/*
	FinalDamage=DamageAmount-ArmorPoint;
	if (FinalDamage<0)
	{
		FinalDamage=0;
	}
	SetHealth(NowHealth - FinalDamage);
	*/
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void AEnemyBase::SetHealth(float NewHealth)
{
	NowHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}

float AEnemyBase::GetHealth()
{
	return NowHealth;
}

float AEnemyBase::GetMaxHealth()
{
	return MaxHealth;
}

void AEnemyBase::DisplayParticle(FVector HitLocation, UParticleSystem* InParticle)
{
	UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(
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

	UMonsterManagerSubSystem* SubSystem = UMonsterManagerSubSystem::Get(this);
	if (SubSystem)
	{
		if (EnemyMonsterType == EMonsterType::Boss)
		{
			AMainGameMode* GM = AMainGameMode::Get(this);
			if (!GM) return;
			
			GM->NotifyBossDead();
		}
		SubSystem->ReturnToPool(this);
	}
	else
	{
		Destroy();	
	}
	
}

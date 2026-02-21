// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileActor.h"

#include "ProjectileStat.h"
#include "Projects.h"
#include "Character/MainCharacter.h"
#include "Components/SphereComponent.h"
#include "Enemy/EnemyBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/ProjectilePoolManager.h"
#include "Tests/AutomationEditorCommon.h"


// Sets default values
AProjectileActor::AProjectileActor()
{
	//루트 스피어 컴포넌트로 충돌 판정을 진행
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAll"));
	SphereComponent->OnComponentHit.AddDynamic(this, &AProjectileActor::OnProjectileHit);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECR_Ignore);
	//스태틱 메시 설정 및 콜리전 해제
	ProjectileStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ProjectileStaticMesh->SetupAttachment(SphereComponent);
	ProjectileStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ProjectileStaticMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECR_Ignore);
	//PMC 컴포넌트 생성
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(SphereComponent);
	//PMC 설정. 디폴트 초기속도 : 1000.f
	ProjectileMovement->InterpLocationTime = 0.05f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	
	//풀링 대기로 멈춰있는다.
	ProjectileMovement->bAutoActivate= false;	
}

void AProjectileActor::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetWorld()==nullptr||GetWorld()->GetSubsystem<UProjectilePoolManager>()==nullptr)
	{
		return;
	}
	if (OtherActor==nullptr||OtherActor==this||OtherActor==Enemy)
	{
		return;
	}
	
	if (TObjectPtr<AMainCharacter> MainCharacter= Cast<AMainCharacter>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(MainCharacter,AttackPoint,nullptr,this,UDamageType::StaticClass());
	}
	GetWorld()->GetSubsystem<UProjectilePoolManager>()->ReturnToPool(this);
	UE_LOG(LogTemp,Warning,TEXT("Destroyed by %s"), *OtherActor->GetName());
}




void AProjectileActor::SetEnemySetting(TObjectPtr<AEnemyBase> InEnemy)
{
	Enemy=InEnemy;
	AttackPoint=InEnemy->GetAttackPoint();
	SphereComponent->IgnoreActorWhenMoving(Enemy, true);
	
	if (ProjectileMovement!=nullptr)
	{
		TSharedPtr<FProjectileStatus>  PStatus=InEnemy->GetProjectileStatus();
		ProjectileMovement->InitialSpeed = PStatus->Speed;
		
		if (PStatus->bIsHoming)
		{
			ProjectileMovement->bIsHomingProjectile = true;
			if (UGameplayStatics::GetPlayerPawn(this, 0))
			{
				//싱글 플레이 전용- 플레이어에게 호밍
				ProjectileMovement->HomingTargetComponent = UGameplayStatics::GetPlayerPawn(this, 0)->GetRootComponent();
			}
			
			ProjectileMovement->HomingAccelerationMagnitude = 2000.f; 
		}
		else
		{
			ProjectileMovement->bIsHomingProjectile=false;
		}
		
		if (PStatus->StaticMesh!=nullptr)
		{
			ProjectileStaticMesh->SetStaticMesh(PStatus->StaticMesh);
		}
		if (PStatus->Material!=nullptr)
		{
			ProjectileStaticMesh->SetOverlayMaterial(PStatus->Material);
		}
		
	}
	
	

}


void AProjectileActor::SetActiveAndCollision(bool InActive)
{

	if (InActive)
	{
		
		SetActorHiddenInGame(false);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		ProjectileMovement->Activate(true);
		
		ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&AProjectileActor::LifeTimeEnd,5.0f,false);
		UE_LOG(LogTemp,Warning,TEXT("Activated"));
		


	}
	else
	{
		SetActorHiddenInGame(true);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		UE_LOG(LogTemp,Warning,TEXT("DeActivated"));
	}
	
}

void AProjectileActor::LifeTimeEnd()
{
	if (GetWorld()==nullptr||GetWorld()->GetSubsystem<UProjectilePoolManager>()==nullptr)
	{
		return;
	}
	GetWorld()->GetSubsystem<UProjectilePoolManager>()->ReturnToPool(this);
	UE_LOG(LogTemp,Warning,TEXT("TimeEnd"));
}


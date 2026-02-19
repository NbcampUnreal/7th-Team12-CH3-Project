// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectileAttackNotify.h"

#include "Enemy/EnemyBase.h"
#include "Enemy/Projectile/ProjectileActor.h"
#include "Manager/ProjectilePoolManager.h"

UEnemyProjectileAttackNotify::UEnemyProjectileAttackNotify()
{
	
}

void UEnemyProjectileAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp==nullptr)
	{
		return;
	}
	
	//발사체를 쏜 적 캐릭터를 가져옵니다.
	TObjectPtr<AEnemyBase> EnemyCharacter=Cast<AEnemyBase>(MeshComp->GetOwner());
	if (MeshComp->GetWorld()==nullptr||MeshComp->GetWorld()->GetSubsystem<UProjectilePoolManager>()==nullptr||EnemyCharacter==nullptr)
	{
		return;
	}
	//오브젝트 풀에서 발사체 하나를 가져옵니다.
	TObjectPtr<AActor> Actor=MeshComp->GetWorld()->GetSubsystem<UProjectilePoolManager>()->GetProjectile();
	if (Actor==nullptr)
	{
		return;
	}
	ProjectileActor=Cast<AProjectileActor>(Actor);
	//발사체의 StaticMesh와 발사체를 쏜 적 캐릭터를 세팅합니다.
	ProjectileActor->SetStaticMesh(EnemyCharacter->GetProjectileStaticMesh());
	ProjectileActor->SetEnemySetting(EnemyCharacter);

	//발사체의 위치와 방향을 세팅합니다.
	FVector SpawnLocation = EnemyCharacter->GetMesh()->GetSocketTransform(FName("Attack_Socket"), RTS_World).
												  GetLocation();
	FRotator SpawnRotation=EnemyCharacter->GetActorRotation();
	ProjectileActor->SetActorLocation(SpawnLocation);
	
	
	ProjectileActor->SetActorRotation(SpawnRotation);
	
	//발사체를 날아가게 합니다
	ProjectileActor->SetActiveAndCollision(true);
	
	UE_LOG(LogTemp,Warning,TEXT("Shoot Projectile"));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectileAttackNotify.h"

#include "Enemy/EnemyBase.h"
#include "Enemy/Projectile/ProjectileActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	//발사체의 Status를 세팅합니다.
	ProjectileActor->SetEnemySetting(EnemyCharacter);

	//발사체의 위치와 방향을 세팅합니다.
	FVector SpawnLocation = EnemyCharacter->GetMesh()->GetSocketTransform(FName("Attack_Socket"), RTS_World).
												  GetLocation();
	
	//기존 바라보는 방향대로 타겟
	//FRotator SpawnRotation=EnemyCharacter->GetActorRotation();
	//싱글 플레이 전용- 플레이어에게 타겟
	FRotator SpawnRotation;
	if (UGameplayStatics::GetPlayerPawn(MeshComp->GetWorld(), 0))
	{
		SpawnRotation = (UGameplayStatics::GetPlayerPawn(MeshComp->GetWorld(),0)->GetActorLocation() - SpawnLocation).Rotation();
		
	}
	
	
	ProjectileActor->SetActorLocation(SpawnLocation);
	
	
	ProjectileActor->SetActorRotation(SpawnRotation);
	
	//발사체를 날아가게 합니다
	ProjectileActor->SetActiveAndCollision(true);
	
	UE_LOG(LogTemp,Warning,TEXT("Shoot Projectile"));
}

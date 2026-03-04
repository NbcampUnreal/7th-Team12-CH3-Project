// Fill out your copyright notice in the Description page of Project Settings.


#include "BossRazorNotifyState.h"

#include "Character/MainCharacter.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/Emitter.h"

void UBossRazorNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}
	OwnerEnemyBase = Cast<AEnemyBase>(Owner);
	if (OwnerEnemyBase == nullptr)
	{
		return;
	}
	if (RazorParticleSystem != nullptr)
	{
		FVector EmitterSpawnLocation=OwnerEnemyBase->GetActorLocation()+FVector(0,0,100.0f)+OwnerEnemyBase->GetActorForwardVector()*500.0f;
		RazorParticleActor = MeshComp->GetWorld()->SpawnActor<AEmitter>(AEmitter::StaticClass(),OwnerEnemyBase->GetActorLocation(), OwnerEnemyBase->GetActorRotation());
		RazorParticleActor->SetActorScale3D(FVector(100.0f,100.0f,100.0f));
		RazorParticleActor->SetTemplate(RazorParticleSystem);
	}
}

void UBossRazorNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(OwnerEnemyBase) || !IsValid(OwnerEnemyBase->GetMesh()))
	{
		return;
	}
	
	//레이저 파티클 회전
	if (RazorParticleActor != nullptr)
	{
		RazorParticleActor->SetActorRotation(OwnerEnemyBase->GetActorRotation()+FRotator(0,180.0f,0));
	}
	
	//Sphere Trace
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerEnemyBase);
	EDrawDebugTrace::Type DebugType = bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		Owner->GetWorld(),
		OwnerEnemyBase->GetActorLocation()+OwnerEnemyBase->GetActorForwardVector()*500,
		OwnerEnemyBase->GetActorLocation()+OwnerEnemyBase->GetActorForwardVector()*RazorDistance,
		RazorTraceRadius,
		//Pawn만 Trace 처리
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, // bTraceComplex
		ActorsToIgnore,
		DebugType,
		HitResults,
		true, // bIgnoreSelf
		FLinearColor::Red, // TraceColor
		FLinearColor::Green, // TraceHitColor
		2.0f // DrawTime
	);
	//피격되지 않았으면 Early Return
	if (!bHit)
	{
		return;
	}
	for (const FHitResult& HitResult : HitResults)
	{
		HittedActor=HitResult.GetActor();
		
		//피격당한 캐릭터를 메인캐릭터로 형변환
		HittedCharacter=Cast<AMainCharacter>(HittedActor);
		if (HittedCharacter == nullptr)
		{
			continue;
		}
		
		//데미지 적용
		UGameplayStatics::ApplyDamage(
		HittedCharacter,
			OwnerEnemyBase->GetAttackPoint(),
	nullptr,
	OwnerEnemyBase,
			UDamageType::StaticClass()
			);
		
		//넉백 추가
		KnockBackDirection=HittedCharacter->GetActorLocation()-OwnerEnemyBase->GetActorLocation();
		KnockBackDirection=FVector(KnockBackDirection.X,KnockBackDirection.Y,0.f);
		KnockBackDirection.Normalize();
		KnockBackDirection*=RazorLaunchPowerXY;
		KnockBackDirection=FVector(KnockBackDirection.X,KnockBackDirection.Y,RazorLaunchPowerZ);
		HittedCharacter->LaunchCharacter(KnockBackDirection,true,true);
	}
}

void UBossRazorNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (RazorParticleActor != nullptr)
	{
		RazorParticleActor->Destroy();
	}
}


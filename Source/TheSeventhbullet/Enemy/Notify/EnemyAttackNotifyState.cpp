// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAttackNotifyState.h"
#include"Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "System/MainGameMode.h"
#include "TheSeventhbullet/Character/MainCharacter.h"
#include "TheSeventhbullet/Enemy/EnemyBase.h"
#include "WorldPartition/HLOD/DestructibleHLODComponent.h"

UEnemyAttackNotifyState::UEnemyAttackNotifyState()
{
	Owner = nullptr;
	bShowDebug = false;
}

void UEnemyAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
	//소켓 위치로 변수 초기화
	CurrentAttackSocketLocation = OwnerEnemyBase->GetMesh()->GetSocketTransform(FName("Attack_Socket"), RTS_World).
	                                              GetLocation();
	PresentAttackSocketLocation = OwnerEnemyBase->GetMesh()->GetSocketTransform(FName("Attack_Socket"), RTS_World).
	                                              GetLocation();
	
	GM = AMainGameMode::Get(MeshComp);
}

void UEnemyAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (!IsValid(OwnerEnemyBase) || !IsValid(OwnerEnemyBase->GetMesh()))
	{
		return;
	}

	CurrentAttackSocketLocation = OwnerEnemyBase->GetMesh()->GetSocketTransform(FName("Attack_Socket"), RTS_World).
	                                              GetLocation();

	//Sphere Trace
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerEnemyBase);
	EDrawDebugTrace::Type DebugType = bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		Owner->GetWorld(),
		PresentAttackSocketLocation,
		CurrentAttackSocketLocation,
		TraceRadius,
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
	//이전 소켓의 위치를 갱신
	PresentAttackSocketLocation = CurrentAttackSocketLocation;
	//피격되지 않았으면 Early Return
	if (!bHit)
	{
		return;
	}
	for (const FHitResult& HitResult : HitResults)
	{
		HittedActor=HitResult.GetActor();
		//1번 공격시 이미 피격당한 목록에 있다면 중복 타격 방지
		if (!HittedActor||HittedCharacterArray.Contains(HittedActor))
		{
			continue;
		}
		//피격당한 캐릭터를 메인캐릭터로 형변환
		HittedCharacter=Cast<AMainCharacter>(HittedActor);
		if (HittedCharacter == nullptr)
		{
			continue;
		}
		
		if (!GM) return;
		GM->RequestHit++;
		
		//피격당한 목록에 추가
		HittedCharacterArray.Add(HittedCharacter);
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
		KnockBackDirection*=LaunchPowerXY;
		KnockBackDirection=FVector(KnockBackDirection.X,KnockBackDirection.Y,LaunchPowerZ);
		HittedCharacter->LaunchCharacter(KnockBackDirection,true,true);
	}
}


void UEnemyAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	//피격당한 목록을 비워준다.
	HittedCharacterArray.Empty();
}

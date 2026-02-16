// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAttackNotifyState.h"
#include"Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TheSeventhbullet/Character/MainCharacter.h"
#include "TheSeventhbullet/Enemy/EnemyBase.h"
#include "WorldPartition/HLOD/DestructibleHLODComponent.h"

UEnemyAttackNotifyState::UEnemyAttackNotifyState()
{
	Owner = nullptr;
	bShowDebug = false;
}

void UEnemyAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
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
}

void UEnemyAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	if (OwnerEnemyBase == nullptr)
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
	PresentAttackSocketLocation = CurrentAttackSocketLocation;
	if (!bHit)
	{
		return;
	}
	for (const FHitResult& HitResult : HitResults)
	{
		HittedActor=HitResult.GetActor();
		if (!HittedActor||HittedCharacterArray.Contains(HittedActor))
		{
			continue;
		}
		HittedCharacter=Cast<AMainCharacter>(HittedActor);
		if (HittedCharacter == nullptr)
		{
			continue;
		}
		HittedCharacterArray.Add(HittedCharacter);
		UGameplayStatics::ApplyDamage(
		HittedCharacter,
			OwnerEnemyBase->GetAttackPoint(),
	nullptr,
	OwnerEnemyBase,
			UDamageType::StaticClass()
			);
	}
}


void UEnemyAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	HittedCharacterArray.Empty();
}

#include "Revolver.h"
#include "Kismet/KismetSystemLibrary.h"

ARevolver::ARevolver()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxAmmo = 6;
	CurrentAmmo = MaxAmmo;
}

void ARevolver::StartFire()
{
	Super::StartFire();
}

void ARevolver::StopFire()
{
	Super::StopFire();
}

void ARevolver::Fire()
{
	Super::Fire();
	
	FVector CameraLocation;
	FRotator CameraRotation;
	WeaponOwner->GetActorEyesViewPoint(CameraLocation, CameraRotation);
	
	FVector Start = CameraLocation;
	UE_LOG(LogTemp, Warning, TEXT("Start : %s"), *Start.ToString());
	FVector End = Start + (CameraRotation.Vector() * Range);
	UE_LOG(LogTemp, Warning, TEXT("End : %s"), *End.ToString());
	
	FHitResult Hit;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(WeaponOwner);

	const EDrawDebugTrace::Type DebugType = bDrawFireDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start,
		End,
		TraceType,
		true,
		ActorsToIgnore,
		DebugType,
		Hit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		FireDebugDuration
	);
	
	if (bHit)
	{
		Hit.GetActor()->Destroy();
		// 명중한 대상에 데미지처리.
	}
}

void ARevolver::Reload()
{
	Super::Reload();
}

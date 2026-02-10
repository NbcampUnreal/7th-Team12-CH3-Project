#include "WeaponBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TheSeventhbullet/Weapon/Data/WeaponDataAsset.h"
#include "Particles/ParticleSystemComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(RootComponent);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetSimulatePhysics(false);
	
	WeaponOwner = nullptr;
}

void AWeaponBase::Initialize(TObjectPtr<UWeaponDataAsset> WeaponData)
{
	WeaponDataAsset = WeaponData;
	
	if (WeaponData->MeshComp)
	{
		Mesh->SetStaticMesh(WeaponDataAsset->MeshComp.LoadSynchronous());
	}
	
	Damage = WeaponDataAsset->BaseDamage;
	FireInterval = WeaponDataAsset->FireInterval;
	Range = WeaponDataAsset->Range;
	MaxAmmo = WeaponDataAsset->MaxAmmo;
	CurrentAmmo = MaxAmmo;	
	ReloadTime = WeaponDataAsset->ReloadTime;
	
	AmountOfPellets = WeaponDataAsset->PelletsCount;
	PelletSpreadRadius = WeaponDataAsset->SpreadRadius;
}

void AWeaponBase::StartFire()
{
	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&AWeaponBase::Fire,
		FireInterval,
		true,
		0
	);
	
	UE_LOG(LogTemp, Warning, TEXT("Start Fire"));
}

void AWeaponBase::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	UE_LOG(LogTemp, Warning, TEXT("StopFire"));
}

void AWeaponBase::Reload()
{
	if (bisReloading)
	{
		return;
	}

	bisReloading = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Start Reload"));
	
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		FTimerDelegate::CreateLambda([this]()
		{
			CurrentAmmo = MaxAmmo;
			bisReloading = false;
			UE_LOG(LogTemp, Warning, TEXT("Reload"));
			UE_LOG(LogTemp, Warning, TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
		}),
		ReloadTime,
		false
	);
}

void AWeaponBase::Fire()
{
	// 총알이 0발 이하로 남았다면 재장전을 자동으로 수행 + 발사하지 않음
	if (CurrentAmmo <= 0)
	{
		Reload();
		return;
	}
	
	// 현재시간 - 마지막 발사시간 < 발사간격인 경우 발사가 불가능
	// 무기마다 발사간격(발사속도)을 구현하기 위한 로직.
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastFireTime < FireInterval)
	{
		return;
	}
	LastFireTime = Now;
	
	// 총알 감소
	ConsumeAmmo();
	
	FVector CameraLocation;
	FRotator CameraRotation;
	WeaponOwner->GetActorEyesViewPoint(CameraLocation, CameraRotation);
	
	FVector Start = CameraLocation;
	FVector MaxTargetLocation = Start + (CameraRotation.Vector() * Range);
	
	FHitResult Hit;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(WeaponOwner);

	const EDrawDebugTrace::Type DebugType = bDrawFireDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	for (int32 i = 0; i < AmountOfPellets; i++)
	{
		// 트레이스 종료지점을 랜덤으로 계산하여 탄 퍼짐을 구현.
		FVector End = TraceRandShot(Start, MaxTargetLocation);
		
		// Start 지점에서 End 지점까지 Linetrace를 수행하고 Hit 여부를 return
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
			// 명중한 곳에 파티클 구현, 추후에 태그를 검사해서 태그에 따른 서로 다른 파티클을 구현할 예정.
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				WeaponDataAsset->ImpactEffect.LoadSynchronous(),
				Hit.ImpactPoint,
				Hit.ImpactNormal.Rotation()
			);
			// 명중한 대상에 데미지처리 로직을 추가할 예정
		}
	}
}

void AWeaponBase::ConsumeAmmo()
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - 1, 0, MaxAmmo);
	UE_LOG(LogTemp, Warning, TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
}

FVector AWeaponBase::TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation)
{
	FVector ToTargetNormalized = (MaxTargetLocation - TraceStart).GetSafeNormal();
	
	// 사거리 끝 지점에 구체의 중심점을 찍음.
	FVector SphereCenter = TraceStart + ToTargetNormalized * Range;
	DrawDebugSphere(GetWorld(), SphereCenter, PelletSpreadRadius, 15.f, FColor::Red, bDrawDebugInfinite,FireDebugDuration);
	
	// 구체 중심을 기준으로 지정된 탄퍼짐 범위(PelletSpreadRadius)만큼의 범위 안에서 타겟 지점을 랜덤으로 정함.
	FVector RandomTarget = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, PelletSpreadRadius);
	// 실제 목표 지점.
	FVector EndLocation = SphereCenter + RandomTarget;
	
	DrawDebugSphere(GetWorld(), EndLocation, 3.f, 15.f, FColor::Emerald, bDrawDebugInfinite,FireDebugDuration);
	DrawDebugLine(GetWorld(), TraceStart, EndLocation, FColor::Magenta, bDrawDebugInfinite,FireDebugDuration);
	
	return EndLocation;
}

void AWeaponBase::Equip(TObjectPtr<AActor> NewOwner)
{
	WeaponOwner = NewOwner;
}

void AWeaponBase::UnEquip()
{
	WeaponOwner = nullptr;
}

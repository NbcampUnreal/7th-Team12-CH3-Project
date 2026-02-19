#include "WeaponBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TheSeventhbullet/DataAsset/WeaponDataAsset.h"
#include "TheSeventhbullet/Character/MainCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Character/Component/CombatComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(RootComponent);
	
	// 테스트용 오버랩 방식 장착
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnItemEndOverlap); 
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetSimulatePhysics(false);	
	
	WeaponOwner = nullptr;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	Initialize(nullptr);	
}

void AWeaponBase::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	EquipWeapon(OtherActor);
}

void AWeaponBase::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AWeaponBase::Initialize(TObjectPtr<APawn> NewOwner)
{
	WeaponOwner = NewOwner;
	
	if (!WeaponDataAsset)
	{
		return;
	}
	
	if (WeaponDataAsset->Mesh)
	{
		MeshComp->SetStaticMesh(WeaponDataAsset->Mesh);
	}
	
	BaseDamage = WeaponDataAsset->BaseDamage;
	Range = WeaponDataAsset->Range;
	AmountOfPellets = WeaponDataAsset->PelletsCount;
	PelletSpreadRadius = WeaponDataAsset->SpreadRadius;
	
	// ProjectileEffect 캐싱
	if (!WeaponDataAsset->ProjectileEffect.ToSoftObjectPath().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileEffect Path Invalid"));
	}
	else
	{
		CachedProjectileEffect = WeaponDataAsset->ProjectileEffect.LoadSynchronous();
	}
}

bool AWeaponBase::PerformTrace(FHitResult& OutHit)
{	
	FVector CameraLocation;
	FRotator CameraRotation;
	WeaponOwner->GetActorEyesViewPoint(CameraLocation, CameraRotation);
	
	FVector Start = CameraLocation;
	FVector MaxTargetLocation = Start + (CameraRotation.Vector() * Range);
	
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(WeaponOwner);

	// 디버그 드로우 여부 판단
	const EDrawDebugTrace::Type DebugType = bDrawFireDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	for (int32 i = 0; i < AmountOfPellets; i++)
	{
		FVector End = TraceRandShot(Start, MaxTargetLocation);
		
		// Start 지점에서 End 지점까지 Trace를 수행하고 Hit 여부를 return
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			Start,
			End,
			TraceType,
			true,
			ActorsToIgnore,
			DebugType,
			OutHit,
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
				OutHit.ImpactPoint,
				OutHit.ImpactNormal.Rotation()
			);
		}
	}
	return true;
}

FVector AWeaponBase::TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation)
{
	// 시작점에서 목표지점까지의 벡터를 정규화(방향만 유지, 크기는 1)
	FVector ToTargetNormalized = (MaxTargetLocation - TraceStart).GetSafeNormal();
	// 사거리 끝 지점에 구체의 중심점을 찍음.
	FVector SphereCenter = TraceStart + ToTargetNormalized * Range;
	// 구체 중심을 기준으로 지정된 탄퍼짐 범위(PelletSpreadRadius)만큼의 범위 안에서 타겟 지점을 랜덤으로 정함.
	FVector RandomTarget = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, PelletSpreadRadius);
	// 실제 목표 지점.
	FVector EndLocation = SphereCenter + RandomTarget;
	
	// 발사체 파티클
	if (CachedProjectileEffect)
	{
		UNiagaraComponent* Projectile = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CachedProjectileEffect,
			TraceStart,
			FRotator::ZeroRotator
		);
		Projectile->SetVectorParameter(FName("Start"), TraceStart);
		Projectile->SetVectorParameter(FName("Target"), EndLocation);
	}
	
	// 디버그 드로우
	// 탄 퍼짐 범위
	//DrawDebugSphere(GetWorld(), SphereCenter, PelletSpreadRadius, 15.f, FColor::Red, bDrawDebugInfinite,FireDebugDuration);
	// 탄환 목표 지점
	//DrawDebugSphere(GetWorld(), EndLocation, 2.f, 15.f, FColor::Emerald, bDrawDebugInfinite,FireDebugDuration);
	// 탄환 경로
	//DrawDebugLine(GetWorld(), TraceStart, EndLocation, FColor::Magenta, bDrawDebugInfinite,FireDebugDuration);
	
	return EndLocation;
}

void AWeaponBase::EquipWeapon(TObjectPtr<AActor> NewWeaponOwner)
{	
	if (!WeaponDataAsset)
	{
		return;
	}
	
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(NewWeaponOwner))
	{
		if (MainCharacter->CurrentWeapon != nullptr)
		{
			MainCharacter->CurrentWeapon->Destroy();
			MainCharacter->CurrentWeapon = nullptr;
		}
		
		Initialize(MainCharacter);
		MainCharacter->CombatComponent->InitializeWeaponData(this);

		AttachToComponent(
			MainCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("Weapon_R")
		);
		
		MainCharacter->CurrentWeapon = this;
		UE_LOG(LogTemp, Warning, TEXT("Equip"));
	}
}
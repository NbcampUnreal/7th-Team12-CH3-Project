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
	FireInterval = WeaponDataAsset->FireInterval;
	Range = WeaponDataAsset->Range;
	MaxAmmo = WeaponDataAsset->MaxAmmo;
	CurrentAmmo = MaxAmmo;	
	ReloadTime = WeaponDataAsset->ReloadTime;
	
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

void AWeaponBase::Reload()
{
	// 재장전 중인지 여부를 판단. 이미 재장전 중이라면 실행하지 않음.
	if (bIsReloading)
	{
		return;
	}

	// 재장전 중 상태로 변경
	bIsReloading = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Start Reload"));
	
	// 재장전 시간(ReloadTime) 이후에 재장전 완료
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		FTimerDelegate::CreateLambda([this]()
		{
			CurrentAmmo = MaxAmmo;
			bIsReloading = false;
			UE_LOG(LogTemp, Warning, TEXT("Reload"));
			UE_LOG(LogTemp, Warning, TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
		}),
		ReloadTime,
		false
	);
}

bool AWeaponBase::PerformTrace(FHitResult& OutHit)
{
	// 총알이 0발 이하로 남았다면 재장전을 자동으로 수행 + 발사하지 않음
	if (CurrentAmmo <= 0)
	{
		Reload();
		return false;
	}
	
	// 현재시간 - 마지막 발사시간 < 발사간격인 경우 발사가 불가능
	// 연사가 아닌 단발사격인 경우에도 무기마다 발사간격(발사속도)을 구현하기 위한 로직.
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastFireTime < FireInterval)
	{
		return false;
	}
	LastFireTime = Now;
	
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
		
		// Start 지점에서 End 지점까지 Linetrace를 수행하고 Hit 여부를 return
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
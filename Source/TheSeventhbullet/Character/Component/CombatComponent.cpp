#include "CombatComponent.h"

#include "EquipmentComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/MainCharacter.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Damage/DamageContext.h"
#include "Damage/Modifier/DamageModifier.h"
#include "Damage/Modifier/WeaponDamageModifier.h"
#include "Damage/Modifier/StatusDamageModifier.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UEquipmentComponent* EC = GetOwner()->FindComponentByClass<UEquipmentComponent>();
	if (EC)
	{
		EC->OnWeaponEquipmentChanged.AddDynamic(this, &UCombatComponent::UCombatComponent::HandleWeaponEquipmentChanged);
	}
	
	DamageModifiersPipeline.Add(NewObject<UWeaponDamageModifier>(this));
	DamageModifiersPipeline.Add(NewObject<UStatusDamageModifier>(this));
}

void UCombatComponent::InitializeWeaponData(UWeaponDataAsset* Weapon)
{
	if (!Weapon)
	{
		return;
	}
	
	WeaponOwner = Cast<AMainCharacter>(GetOwner());
	WeaponDataView = Weapon;
	CurrentWeaponStatus.WeaponBaseDamage = WeaponDataView->BaseDamage;
	CurrentWeaponStatus.Range = WeaponDataView->Range;
	CurrentWeaponStatus.AmountOfPellets = WeaponDataView->PelletsCount;
	CurrentWeaponStatus.PelletSpreadRadius = WeaponDataView->SpreadRadius;
	CurrentWeaponStatus.IncreaseSpreadRadiusValue = WeaponDataView->IncreaseSpreadRadius;
	CurrentWeaponStatus.FireInterval = WeaponDataView->FireInterval;
	CurrentWeaponStatus.MaxAmmo = WeaponDataView->MaxAmmo;
	CurrentWeaponStatus.ReloadTime = WeaponDataView->ReloadTime;
	CurrentWeaponStatus.MaxAmmo = WeaponDataView->MaxAmmo;
	
	CurrentAmmo = CurrentWeaponStatus.MaxAmmo;
}

void UCombatComponent::HandleWeaponEquipmentChanged(UWeaponDataAsset* NewWeaponData)
{
	InitializeWeaponData(NewWeaponData);
}

void UCombatComponent::StartFire()
{
	if (!WeaponDataView)
	{
		return;
	}
	
	bIsFiring = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Start Fire"));

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&UCombatComponent::HitScanFire,
		CurrentWeaponStatus.FireInterval,
		true,
		0.02
	);
}

void UCombatComponent::StopFire()
{
	bIsFiring = false;
	UE_LOG(LogTemp, Warning, TEXT("StopFire"));
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	ResetSpreadRadius();
}

void UCombatComponent::HitScanFire()
{	
	// 총알이 0발 이하로 남았다면 재장전을 자동으로 수행 + 발사하지 않음
	if (CurrentAmmo <= 0)
	{
		Reload();
		return;
	}
	
	// 현재시간 - 마지막 발사시간 < 발사간격인 경우 발사가 불가능
	// 연사가 아닌 단발사격인 경우에도 무기마다 발사간격(발사속도)을 구현하기 위한 로직.
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastFireTime < CurrentWeaponStatus.FireInterval)
	{
		return;
	}
	LastFireTime = Now;
	
	FHitResult Hit;
	PerformTrace(Hit);
	SpreadBullet();
	SpawnFireParticles(Hit);
	ConsumeAmmo();
	
	if (Hit.bBlockingHit && Hit.GetActor()->ActorHasTag("Enemy"))
	{
		ApplyDamageByHit(Hit);
	}
}

void UCombatComponent::PerformTrace(FHitResult& OutHit)
{
	// 1) 카메라를 기준으로 타겟지점을 정함.
	FVector CameraLocation;
	FRotator CameraRotation;
	GetOwner()->GetActorEyesViewPoint(CameraLocation, CameraRotation);
	
	FVector AimStart = CameraLocation;
	FVector AimEnd = AimStart + (CameraRotation.Vector() * CurrentWeaponStatus.Range);
	
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	
	// 디버그 드로우 여부 판단
	const EDrawDebugTrace::Type DebugType = bDrawFireDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	
	// 타겟지점을 정하는 라인트레이스 : 카메라 -> 타겟지점
	FHitResult AimHit;
	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		AimStart,
		AimEnd,
		TraceType,
		true,
		ActorsToIgnore,
		DebugType,
		AimHit,
		true
	);
	// 타겟지점
	const FVector AimPoint = AimHit.bBlockingHit ? AimHit.ImpactPoint : AimEnd;
	
	
	// 2) 총구에서 실제로 발사하는 트레이스
	FVector MuzzleLoc;
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(GetOwner()))
	{
		if (MainCharacter->WeaponMeshComponent->DoesSocketExist(TEXT("WeaponMuzzle")))
		{
			// 소켓이 있으면 소켓 위치로
			MuzzleLoc = MainCharacter->WeaponMeshComponent->GetSocketLocation(TEXT("WeaponMuzzle"));
		}
		else
		{
			// 없으면 일단 무기 위치로
			MuzzleLoc = MainCharacter->WeaponMeshComponent->GetComponentLocation();
		}
	}
	
	bool bHasBestHit = false;
	FHitResult BestHit;
	FHitResult LastHit;
	
	// 펠릿 수 만큼 트레이스
	for (int32 i = 0; i < CurrentWeaponStatus.AmountOfPellets; i++)
	{
		// Muzzle 위치에서 AimPoint까지 트레이스
		FVector End = TraceRandShot(MuzzleLoc, AimPoint);
		FHitResult PelletHit;
		bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			MuzzleLoc,
			End,
			TraceType,
			true,
			ActorsToIgnore,
			DebugType,
			PelletHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			FireDebugDuration
		);
		
		LastHit = PelletHit;
		
		if (bHit)
		{
			if (!bHasBestHit || PelletHit.Distance < LastHit.Distance)
			{
				BestHit = PelletHit;
				bHasBestHit = true;
			}
		}
	}
	
	OutHit = bHasBestHit ? BestHit : LastHit;
}

FVector UCombatComponent::TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation)
{
	// 시작점에서 목표지점까지의 벡터를 정규화(방향만 유지, 크기는 1)
	FVector ToTargetNormalized = (MaxTargetLocation - TraceStart).GetSafeNormal();
	
	// MaxTargetLocation까지의 거리 기준으로 구체 중심을 잡는다. (기본 AimTrace가 Range 끝을 주면 결과적으로 Range와 동일)
	const float TraceDistance = FMath::Max((MaxTargetLocation - TraceStart).Size(), 1.f);
	// 거리 끝 지점에 구체의 중심점을 찍음.
	FVector SphereCenter = TraceStart + ToTargetNormalized * TraceDistance;
	
	// 구체 중심을 기준으로 지정된 탄퍼짐 범위(PelletSpreadRadius)만큼의 범위 안에서 타겟 지점을 랜덤으로 정함.
	FVector RandomTarget = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, CurrentWeaponStatus.PelletSpreadRadius);
	// 실제 목표 지점.
	FVector EndLocation = SphereCenter + RandomTarget;
	
	// 디버그 드로우
	// 탄 퍼짐 범위
	//DrawDebugSphere(GetWorld(), SphereCenter, PelletSpreadRadius, 15.f, FColor::Red, bDrawDebugInfinite,FireDebugDuration);
	// 탄환 목표 지점
	//DrawDebugSphere(GetWorld(), EndLocation, 2.f, 15.f, FColor::Emerald, bDrawDebugInfinite,FireDebugDuration);
	// 탄환 경로
	//DrawDebugLine(GetWorld(), TraceStart, EndLocation, FColor::Magenta, bDrawDebugInfinite,FireDebugDuration);
	
	return EndLocation;
}

void UCombatComponent::SpreadBullet()
{
	if (WeaponDataView->WeaponType != EWeaponTypes::ShotGun)
	{
		CurrentWeaponStatus.PelletSpreadRadius = FMath::Clamp(
			CurrentWeaponStatus.PelletSpreadRadius+CurrentWeaponStatus.IncreaseSpreadRadiusValue, 
			WeaponDataView->SpreadRadius,
			WeaponDataView->MaxSpreadRadius);
	}
}

void UCombatComponent::ResetSpreadRadius()
{
	CurrentWeaponStatus.PelletSpreadRadius = WeaponDataView->SpreadRadius;
}

void UCombatComponent::Reload()
{
	if (bIsReloading)
	{
		return;
	}
	
	bIsReloading = true;
	UE_LOG(LogTemp, Warning, TEXT("Start Reload"));
	
	// 재장전 시간(ReloadTime) 이후에 재장전 완료
	GetWorld()->GetTimerManager().SetTimer(
	ReloadTimerHandle,
	FTimerDelegate::CreateLambda([this]()
	{
		CurrentAmmo = CurrentWeaponStatus.MaxAmmo;
		bIsReloading = false;
		UE_LOG(LogTemp, Warning, TEXT("Reload"));
		UE_LOG(LogTemp, Warning, TEXT("%d / %d"), CurrentAmmo, CurrentWeaponStatus.MaxAmmo);
	}),
	CurrentWeaponStatus.ReloadTime,
	false
);
}

void UCombatComponent::ConsumeAmmo()
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - 1, 0, CurrentWeaponStatus.MaxAmmo);
	UE_LOG(LogTemp, Warning, TEXT("%d / %d"), CurrentAmmo, CurrentWeaponStatus.MaxAmmo);
}


void UCombatComponent::ApplyDamageByHit(const FHitResult& Hit)
{
	FDamageContext Context;
	Context.Attacker = GetOwner();
	Context.Target = Hit.GetActor();
	Context.HitResult = Hit;
	Context.CurrentDamage = CurrentWeaponStatus.WeaponBaseDamage;
	
	ExecutePipeline(Context);

	UGameplayStatics::ApplyPointDamage(
		Context.Target,
		Context.CurrentDamage,
		Context.Attacker->GetActorForwardVector(),
		Hit,
		Hit.GetActor()->GetInstigatorController(),
		Context.Attacker,
		UDamageType::StaticClass()
	);
}

void UCombatComponent::ExecutePipeline(FDamageContext& Context)
{
	for (UDamageModifier* Modifier : DamageModifiersPipeline)
	{
		if (Modifier)
		{
			Modifier->ModifyDamage(Context);
		}
	}
}

void UCombatComponent::SpawnFireParticles(const FHitResult& Hit)
{
	if (WeaponDataView->MuzzleFlashEffect.ToSoftObjectPath().IsValid())
	{
		const FVector EffectDirection = (Hit.TraceStart - Hit.TraceEnd).GetSafeNormal();
		const FRotator EffectRotation = EffectDirection.Rotation();
		
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			WeaponDataView->MuzzleFlashEffect.LoadSynchronous(),
			Hit.TraceStart,
			EffectRotation,
			FVector(0.3),
			true
		);
	}
	
	if (WeaponDataView->ImpactEffect.ToSoftObjectPath().IsValid())
	{		
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			WeaponDataView->ImpactEffect.LoadSynchronous(),
			Hit.ImpactPoint,
			FRotator::ZeroRotator,
			true
		);
	}

	if (WeaponDataView->ProjectileEffect.ToSoftObjectPath().IsValid())
	{
		UNiagaraComponent* Projectile = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			WeaponDataView->ProjectileEffect.LoadSynchronous(),
			Hit.TraceStart,
			FRotator::ZeroRotator
		);
		Projectile->SetVectorParameter(FName("Start"), Hit.TraceStart);
		Projectile->SetVectorParameter(FName("Target"), Hit.TraceEnd);
	}
}



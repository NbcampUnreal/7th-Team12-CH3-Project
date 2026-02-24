#include "CombatComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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

void UCombatComponent::InitializeWeaponData(UWeaponDataAsset* Weapon)
{
	if (!Weapon)
	{
		return;
	}
	
	WeaponData = Weapon;
	
	WeaponBaseDamage = WeaponData->BaseDamage;
	Range = WeaponData->Range;
	AmountOfPellets = WeaponData->PelletsCount;
	PelletSpreadRadius = WeaponData->SpreadRadius;
	IncreaseSpreadRadiusValue = WeaponData->IncreaseSpreadRadius;
	FireInterval = WeaponData->FireInterval;
	MaxAmmo = WeaponData->MaxAmmo;
	ReloadTime = WeaponData->ReloadTime;
	MaxAmmo = WeaponData->MaxAmmo;
	
	CurrentAmmo = MaxAmmo;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	DamageModifiersPipeline.Add(NewObject<UWeaponDamageModifier>(this));
	DamageModifiersPipeline.Add(NewObject<UStatusDamageModifier>(this));
}

void UCombatComponent::StartFire()
{
	if (!WeaponData)
	{
		return;
	}
	
	bIsFiring = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Start Fire"));

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&UCombatComponent::HitScanFire,
		FireInterval,
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
	if (!WeaponData)
	{
		return;
	}
	
	// 총알이 0발 이하로 남았다면 재장전을 자동으로 수행 + 발사하지 않음
	if (CurrentAmmo <= 0)
	{
		Reload();
		return;
	}
	
	// 현재시간 - 마지막 발사시간 < 발사간격인 경우 발사가 불가능
	// 연사가 아닌 단발사격인 경우에도 무기마다 발사간격(발사속도)을 구현하기 위한 로직.
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastFireTime < FireInterval)
	{
		return;
	}
	LastFireTime = Now;
	
	FHitResult Hit;
	PerformTrace(Hit);
	SpreadBullet();
	ApplyDamageByHit(Hit);
	SpawnFireParticles(Hit);
	ConsumeAmmo();
}

void UCombatComponent::PerformTrace(FHitResult& OutHit)
{
	FVector CameraLocation;
	FRotator CameraRotation;
	GetOwner()->GetActorEyesViewPoint(CameraLocation, CameraRotation);
	
	FVector Start = CameraLocation;
	FVector MaxTargetLocation = Start + (CameraRotation.Vector() * Range);
	
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	// 디버그 드로우 여부 판단
	const EDrawDebugTrace::Type DebugType = bDrawFireDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	
	for (int32 i = 0; i < AmountOfPellets; i++)
	{
		FVector End = TraceRandShot(Start, MaxTargetLocation);
		
		// Start 지점에서 End 지점까지 Trace를 수행하고 Hit 여부를 return
		bool bHit = UKismetSystemLibrary::LineTraceSingle(
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
	}
}

FVector UCombatComponent::TraceRandShot(const FVector& TraceStart, const FVector& MaxTargetLocation)
{
	// 시작점에서 목표지점까지의 벡터를 정규화(방향만 유지, 크기는 1)
	FVector ToTargetNormalized = (MaxTargetLocation - TraceStart).GetSafeNormal();
	// 사거리 끝 지점에 구체의 중심점을 찍음.
	FVector SphereCenter = TraceStart + ToTargetNormalized * Range;
	// 구체 중심을 기준으로 지정된 탄퍼짐 범위(PelletSpreadRadius)만큼의 범위 안에서 타겟 지점을 랜덤으로 정함.
	FVector RandomTarget = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, PelletSpreadRadius);
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
	if (WeaponData->WeaponType != EWeaponTypes::ShotGun)
	{
		PelletSpreadRadius = FMath::Clamp(
			PelletSpreadRadius+IncreaseSpreadRadiusValue, 
			WeaponData->SpreadRadius,
			WeaponData->MaxSpreadRadius);
	}
}

void UCombatComponent::ResetSpreadRadius()
{
	PelletSpreadRadius = WeaponData->SpreadRadius;
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
		CurrentAmmo = MaxAmmo;
		bIsReloading = false;
		UE_LOG(LogTemp, Warning, TEXT("Reload"));
		UE_LOG(LogTemp, Warning, TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
	}),
	ReloadTime,
	false
);
}

void UCombatComponent::ConsumeAmmo()
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - 1, 0, MaxAmmo);
	UE_LOG(LogTemp, Warning, TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
}


void UCombatComponent::ApplyDamageByHit(const FHitResult& Hit)
{
	FDamageContext Context;
	Context.Attacker = GetOwner();
	Context.Target = Hit.GetActor();
	Context.HitResult = Hit;
	Context.CurrentDamage = WeaponBaseDamage;
	
	ExecutePipeline(Context);
	
	if (!Context.Target)
	{
		return;
	}
	
	if (Context.Target->ActorHasTag("Enemy"))
	{
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
	if (!WeaponData->ProjectileEffect.ToSoftObjectPath().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileEffect is null"));
		return;
	}
	else
	{
		if (WeaponData->ImpactEffect.ToSoftObjectPath().IsValid())
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				WeaponData->ImpactEffect.LoadSynchronous(),
				Hit.ImpactPoint,
				FRotator::ZeroRotator,
				true
			);
		}
		
		if (WeaponData->ProjectileEffect.ToSoftObjectPath().IsValid())
		{
			UNiagaraComponent* Projectile = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				WeaponData->ProjectileEffect.LoadSynchronous(),
				Hit.TraceStart,
				FRotator::ZeroRotator
			);
			Projectile->SetVectorParameter(FName("Start"), Hit.TraceStart);
			Projectile->SetVectorParameter(FName("Target"), Hit.TraceEnd);
		}
	}
}



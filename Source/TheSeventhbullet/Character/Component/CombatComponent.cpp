#include "CombatComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Damage/Modifier/WeaponDamageModifier.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Damage/DamageContext.h"
#include "Damage/Modifier/DamageModifier.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/WeaponBase.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::InitializeWeaponData(AWeaponBase* Weapon)
{
	if (!Weapon)
	{
		return;
	}
	
	CurrentWeapon = Weapon;
	WeaponData = Weapon->GetWeaponDataAsset();
	
	FireInterval = WeaponData->FireInterval;
	MaxAmmo = WeaponData->MaxAmmo;
	CurrentAmmo = MaxAmmo;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	DamageModifiersPipeline.Add(NewObject<UWeaponDamageModifier>(this));
}

void UCombatComponent::StartFire()
{
	if (!CurrentWeapon)
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
	CurrentWeapon->ResetSpreadRadius();
}

void UCombatComponent::HitScanFire()
{
	if (!CurrentWeapon)
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
	CurrentWeapon->PerformTrace(Hit);
	CurrentWeapon->SpreadBullet();
	ApplyDamageByHit(CurrentWeapon, Hit);
	SpawnFireParticles(Hit);
	ConsumeAmmo();
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


void UCombatComponent::ApplyDamageByHit(AWeaponBase* Weapon, const FHitResult& Hit)
{
	FDamageContext Context;
	Context.Attacker = GetOwner();
	Context.Target = Hit.GetActor();
	Context.Weapon = Weapon;
	Context.HitResult = Hit;
	
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



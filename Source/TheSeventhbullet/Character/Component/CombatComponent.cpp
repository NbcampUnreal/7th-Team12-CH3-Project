#include "CombatComponent.h"

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
	PelletSpreadRadius = WeaponData->SpreadRadius;
	IncreaseSpreadRadiusValue = WeaponData->IncreaseSpreadRadius;
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
}

void UCombatComponent::HitScanFire()
{
	if (!CurrentWeapon)
	{
		return;
	}
	
	FHitResult Hit;
	CurrentWeapon->PerformTrace(Hit);
	ApplyDamageByHit(CurrentWeapon, Hit);
	DrawFireParticles(Hit);
	SpreadBullet();
	ConsumeAmmo();
}

void UCombatComponent::ConsumeAmmo()
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - 1, 0, MaxAmmo);
	PelletSpreadRadius = WeaponData->SpreadRadius;
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

void UCombatComponent::DrawFireParticles(const FHitResult& Hit)
{
	
}

void UCombatComponent::SpreadBullet()
{
	if (WeaponData->WeaponType != EWeaponTypes::ShotGun)
	{
		PelletSpreadRadius = FMath::Clamp(
			PelletSpreadRadius + IncreaseSpreadRadiusValue, 
			WeaponData->SpreadRadius,
			WeaponData->MaxSpreadRadius);
	}
}



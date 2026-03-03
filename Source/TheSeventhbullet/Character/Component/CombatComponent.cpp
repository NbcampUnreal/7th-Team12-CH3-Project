#include "CombatComponent.h"

#include "EquipmentComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Chaos/Utilities.h"
#include "Character/MainCharacter.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Damage/DamageContext.h"
#include "Damage/Modifier/DamageModifier.h"
#include "Damage/Modifier/WeaponDamageModifier.h"
#include "Damage/Modifier/StatusDamageModifier.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/DamageNumberActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Manager/SoundManager.h"
#include "System/MainGameMode.h"

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
		EC->OnWeaponEquipmentChanged.AddDynamic(this, &UCombatComponent::HandleWeaponEquipmentChanged);
	}
	
	DamageModifiersPipeline.Add(NewObject<UStatusDamageModifier>(this));
	DamageModifiersPipeline.Add(NewObject<UWeaponDamageModifier>(this));
	GM = AMainGameMode::Get(this);
}

void UCombatComponent::InitializeWeaponData(UWeaponDataAsset* Weapon)
{
	if (!Weapon)
	{
		return;
	}
	
	WeaponOwner = Cast<AMainCharacter>(GetOwner());
	WeaponDataView = Weapon;
	CurrentWeaponStatus.WeaponDamageMultiplier = WeaponDataView->WeaponDamageMultiplier;
	CurrentWeaponStatus.WeaponCritChance = WeaponDataView->WeaponCritChanceBalance;
	CurrentWeaponStatus.WeaponCritDamage = WeaponDataView->WeaponCritDamageBalance;
	CurrentWeaponStatus.Range = WeaponDataView->Range;
	CurrentWeaponStatus.AmountOfPellets = WeaponDataView->PelletsCount;
	CurrentWeaponStatus.PelletSpreadRadius = WeaponDataView->SpreadRadius;
	CurrentWeaponStatus.IncreaseSpreadRadiusValue = WeaponDataView->IncreaseSpreadRadius;
	CurrentWeaponStatus.FireInterval = WeaponDataView->FireInterval;
	CurrentWeaponStatus.MaxAmmo = WeaponDataView->MaxAmmo;
	CurrentWeaponStatus.ReloadTime = WeaponDataView->ReloadTime;
	CurrentWeaponStatus.MaxAmmo = WeaponDataView->MaxAmmo;
	
	CurrentWeaponStatus.MuzzleFlashEffect = WeaponDataView->MuzzleFlashEffect.LoadSynchronous();
	CurrentWeaponStatus.ImpactEffect = WeaponDataView->ImpactEffect.LoadSynchronous();
	
	CurrentAmmo = CurrentWeaponStatus.MaxAmmo;
	OnAmmoChanged.Broadcast(CurrentAmmo, CurrentWeaponStatus.MaxAmmo);
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
	HitScanFire();
	if (WeaponOwner->EquipmentComponent->FindSpecialOption(ESpecialOptions::DoubleShot))
	{
		HitScanFire();
	}
	
}

void UCombatComponent::StopFire()
{
	bIsFiring = false;
	UE_LOG(LogTemp, Warning, TEXT("StopFire"));
	//GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
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
	// const float Now = GetWorld()->GetTimeSeconds();
	// if (Now - LastFireTime < CurrentWeaponStatus.FireInterval)
	// {
	// 	return;
	// }
	// LastFireTime = Now;
	
	SpreadBullet();
	if (WeaponOwner->EquipmentComponent->FindSpecialOption(ESpecialOptions::BloodBullet))
	{
		float HP = WeaponOwner->GetCurrentHP();
		if (HP <= 2.0f)
		{
			return;
		}
		HP = FMath::Clamp(HP - 2.0f, 1.f, WeaponOwner->GetTotalStatus().HP);
		WeaponOwner->SetCurrentHP(HP);
		WeaponOwner->OnHPChanged.Broadcast(HP, static_cast<float>(WeaponOwner->GetTotalStatus().HP));
	}
	else
	{
		ConsumeAmmo();
	}
	SpawnFireParticles();
	SpawnFireSound();
	TArray<FHitResult> Hits;
	PerformTrace(Hits);
	for (const FHitResult& Hit : Hits)
	{
		SpawnHitParticles(Hit);
		
		if (!Hit.bBlockingHit) continue;
		
		if (Hit.bBlockingHit && Hit.GetActor()->ActorHasTag("Enemy"))
		{
			if (GM)
			{
				GM->RequestAttack++;
				ApplyDamageByHit(Hit);
			}	
		}
	}
}

void UCombatComponent::PerformTrace(TArray<FHitResult>& OutHits)
{
	// 실제 카메라 위치를 기준으로 타겟지점을 정함.
	FVector CameraLocation;
	FRotator CameraRotation;
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* PC = OwnerPawn->GetController<APlayerController>())
		{
			PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
		}
	}
	
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
	
	
	// 펠릿 수 만큼 트레이스 (카메라 위치에서 발사)
	for (int32 i = 0; i < CurrentWeaponStatus.AmountOfPellets; i++)
	{
		FVector End = TraceRandShot(AimStart, AimPoint);
		FHitResult PelletHit;
		bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			AimStart,
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
		
		if (bHit)
		{
			OutHits.Add(PelletHit);
		}
	}
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
	
	// 탄 퍼짐 최종 지점의 방향만 사용해서 해당방향으로 사거리만큼 뻗어서 반드시 히트되게 만든다.
	FVector ShotDirection = (EndLocation - TraceStart).GetSafeNormal();
	return TraceStart + ShotDirection * CurrentWeaponStatus.Range;
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

int UCombatComponent::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int UCombatComponent::GetMaxAmmo() const
{
	return CurrentWeaponStatus.MaxAmmo;
}

void UCombatComponent::Reload()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Reload"));
	CurrentAmmo = CurrentWeaponStatus.MaxAmmo;
	OnAmmoChanged.Broadcast(CurrentAmmo, CurrentWeaponStatus.MaxAmmo);
}

void UCombatComponent::ConsumeAmmo()
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - 1, 0, CurrentWeaponStatus.MaxAmmo);
	OnAmmoChanged.Broadcast(CurrentAmmo, CurrentWeaponStatus.MaxAmmo);
}


void UCombatComponent::ApplyDamageByHit(const FHitResult& Hit)
{
	FDamageContext Context;
	Context.Attacker = GetOwner();
	Context.Target = Hit.GetActor();
	Context.HitResult = Hit;;
	Context.WeaponCritChanceBalance = CurrentWeaponStatus.WeaponCritChance;
	Context.WeaponCritDamageBalance = CurrentWeaponStatus.WeaponCritDamage;
	Context.DamageMultiplier = CurrentWeaponStatus.WeaponDamageMultiplier;
	
	ExecutePipeline(Context);
	
	Context.CurrentDamage *= Context.DamageMultiplier;
	Context.CurrentCritChance = Context.CurrentCritChance + Context.StatusCritChance;
	Context.CurrentCritDamage = Context.CurrentDamage*(Context.CurrentCritDamage+Context.StatusCritDamage);
	
	float IsCritValue = FMath::FRand();
	bool bIsCrit = false;
	
	if (IsCritValue <= Context.CurrentCritChance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Critical] CritChance = %f, CritDamage = %f"), Context.CurrentCritChance, Context.CurrentCritDamage);
		Context.CurrentDamage = Context.CurrentCritDamage;
		bIsCrit = true;
	}
	if (WeaponOwner->EquipmentComponent->FindSpecialOption(ESpecialOptions::BloodBullet))
	{
		Context.CurrentDamage *= 1.4f;
		Context.CurrentCritDamage += 0.5;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Damage : %f"), Context.CurrentDamage);
	
	UGameplayStatics::ApplyPointDamage(
		Context.Target,
		Context.CurrentDamage,
		Context.Attacker->GetActorForwardVector(),
		Hit,
		Hit.GetActor()->GetInstigatorController(),
		Context.Attacker,
		UDamageType::StaticClass()
	);

	if (DamageNumberActorClass)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(Context.Target);
		if (Enemy && Enemy->IsDead()) { /* 이미 죽은 적은 데미지 표시 안 함 */ }
		else
		{
			float DisplayDamage = Context.CurrentDamage;
			bool bIsHeadShot = (Hit.BoneName == TEXT("head"));

			if (Enemy)
			{
				float FinalDamage = bIsHeadShot ? Context.CurrentDamage * 1.5f : Context.CurrentDamage;
				DisplayDamage = FMath::Max(FinalDamage - Enemy->GetArmorPoint(), 0.f);
			}

			FVector SpawnLocation = Hit.ImpactPoint + FVector(0.f, 0.f, 30.f);
			ADamageNumberActor* DamageNumber = GetWorld()->SpawnActor<ADamageNumberActor>(
				DamageNumberActorClass, SpawnLocation, FRotator::ZeroRotator);
			if (DamageNumber)
			{
				DamageNumber->Init(DisplayDamage, bIsCrit);
			}
		}
	}
	
	if (WeaponOwner->EquipmentComponent->FindSpecialOption(ESpecialOptions::Lifesteal))
	{
		float HP = WeaponOwner->GetCurrentHP();
		HP = FMath::Clamp(HP + Context.CurrentDamage*0.1, 0.f, WeaponOwner->GetTotalStatus().HP);
		WeaponOwner->SetCurrentHP(HP);
		WeaponOwner->OnHPChanged.Broadcast(HP, static_cast<float>(WeaponOwner->GetTotalStatus().HP));
	}
	if (WeaponOwner->EquipmentComponent->FindSpecialOption(ESpecialOptions::Heartsteal))
	{
		float HP = WeaponOwner->GetCurrentHP();
		HP = FMath::Clamp(HP + Context.CurrentDamage*0.2, 0.f, WeaponOwner->GetTotalStatus().HP);
		WeaponOwner->SetCurrentHP(HP);
		WeaponOwner->OnHPChanged.Broadcast(HP, static_cast<float>(WeaponOwner->GetTotalStatus().HP));
	}

	OnCurrentDamageBroadcast.Broadcast(Context.CurrentDamage, bIsCrit);

	bool bIsHeadShot = (Hit.BoneName == TEXT("head"));
	OnHitConfirm.Broadcast(bIsHeadShot);
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

void UCombatComponent::SpawnFireParticles()
{
	if (WeaponDataView->MuzzleFlashEffect.ToSoftObjectPath().IsValid())
	{		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CurrentWeaponStatus.MuzzleFlashEffect,
			WeaponOwner->WeaponMeshComponent->GetSocketLocation("WeaponMuzzle"),
			WeaponOwner->WeaponMeshComponent->GetSocketRotation("WeaponMuzzle"),
			FVector(1),
			true
		);
	}
}

void UCombatComponent::SpawnHitParticles(const FHitResult& Hit)
{	
	if (!Hit.bBlockingHit) return;
	if (!CurrentWeaponStatus.ImpactEffect) return;

	FVector SpawnLocation = Hit.ImpactPoint;

	// 트레이스가 충돌체 내부에서 시작한 케이스 보정
	if (Hit.bStartPenetrating)
	{
		// Location이 더 안정적인 경우가 많음
		SpawnLocation = Hit.Location;

		// 혹은 노말 방향으로 살짝 밖으로 빼기(겹침 방지)
		SpawnLocation += Hit.ImpactNormal * 2.f;
	}
	else
	{
		// 표면에 살짝 띄우기 (메시 내부 스폰 방지)
		SpawnLocation += Hit.ImpactNormal * 1.f;
	}

	const FRotator SpawnRotation = Hit.ImpactNormal.Rotation();

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		CurrentWeaponStatus.ImpactEffect,
		SpawnLocation,
		SpawnRotation,
		FVector(1.f),
		true
	);
}

void UCombatComponent::SpawnFireSound()
{
	USoundManager* SoundMgr = USoundManager::Get(GetWorld());
	if (SoundMgr)
	{
		switch(WeaponDataView->WeaponType)
		{
		case EWeaponTypes::HandGun:
			SoundMgr->PlaySoundAttached(TEXT("Fire_Revolver"), WeaponOwner->WeaponMeshComponent);
			break;
		case EWeaponTypes::AssaultRifle:
			SoundMgr->PlaySoundAttached(TEXT("Fire_Rifle"), WeaponOwner->WeaponMeshComponent);
			break;
		case EWeaponTypes::ShotGun:
			SoundMgr->PlaySoundAttached(TEXT("Fire_Shotgun"), WeaponOwner->WeaponMeshComponent);
			break;
		case EWeaponTypes::None:
			return;
		}
	}
}

void UCombatComponent::SpawnReloadSound()
{
	USoundManager* SoundMgr = USoundManager::Get(GetWorld());
	if (SoundMgr)
	{
		switch(WeaponDataView->WeaponType)
		{
		case EWeaponTypes::HandGun:
			SoundMgr->PlaySoundAttached(TEXT("Reload_Revolver"), WeaponOwner->WeaponMeshComponent);
			break;
		case EWeaponTypes::AssaultRifle:
			SoundMgr->PlaySoundAttached(TEXT("Reload_Rifle"), WeaponOwner->WeaponMeshComponent);
			break;
		case EWeaponTypes::ShotGun:
			SoundMgr->PlaySoundAttached(TEXT("Reload_Shotgun"), WeaponOwner->WeaponMeshComponent);
			break;
		case EWeaponTypes::None:
			return;
		}
	}
}



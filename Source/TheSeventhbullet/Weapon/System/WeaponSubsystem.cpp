#include "WeaponSubsystem.h"

#include "InterchangeResult.h"
#include "TheSeventhbullet/Weapon/WeaponBase.h"

void UWeaponSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	WeaponStates.Add(EWeaponTypes::HandGun, FWeaponState());
	WeaponStates.Add(EWeaponTypes::AssaultRifle, FWeaponState());
	WeaponStates.Add(EWeaponTypes::ShotGun, FWeaponState());
	
	// 기본무기(리볼버, 핸드건) 하나는 일단 해금.
	WeaponStates[EWeaponTypes::HandGun].bUnlock = true;
	SelectedWeapon = EWeaponTypes::HandGun;
}

bool UWeaponSubsystem::IsUnlock(EWeaponTypes WeaponType) const
{
	if (const FWeaponState* State = WeaponStates.Find(WeaponType))
	{
		return State->bUnlock;
	}
	
	return false;
}

void UWeaponSubsystem::UnlockWeapon(EWeaponTypes WeaponType)
{
	if (FWeaponState* State = WeaponStates.Find(WeaponType))
	{
		State->bUnlock = true;
	}
}

void UWeaponSubsystem::SelectWeapon(EWeaponTypes WeaponType)
{
	if (!IsUnlock(WeaponType))
	{
		UE_LOG(LogTemp, Warning, TEXT("Locked Weapon"));
		return;
	}
	
	SelectedWeapon = WeaponType;
	
}

EWeaponTypes UWeaponSubsystem::GetSelectedWeapon() const
{
	return SelectedWeapon;
}

FWeaponState* UWeaponSubsystem::GetWeaponState(EWeaponTypes WeaponType)
{
	return WeaponStates.Find(WeaponType);
}

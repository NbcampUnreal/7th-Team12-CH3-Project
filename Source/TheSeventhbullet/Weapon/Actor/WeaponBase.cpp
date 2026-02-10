#include "WeaponBase.h"

#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(RootComponent);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetSimulatePhysics(false);
	
	CurrentAmmo = MaxAmmo;
	WeaponOwner = nullptr;
}

void AWeaponBase::Equip(TObjectPtr<AActor> NewOwner)
{
	WeaponOwner = NewOwner;
}

void AWeaponBase::UnEquip()
{
	WeaponOwner = nullptr;
}

void AWeaponBase::StartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Fire"));
}

void AWeaponBase::StopFire()
{
	UE_LOG(LogTemp, Warning, TEXT("StopFire"));
}

void AWeaponBase::Reload()
{
	CurrentAmmo = MaxAmmo;
}

void AWeaponBase::Fire()
{
	if (CurrentAmmo <= 0)
	{
		return;
	}
	
	ConsumeAmmo();
}

void AWeaponBase::ConsumeAmmo()
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + 1, 0, MaxAmmo);
}

#include "PlayerSkill.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


APlayerSkill::APlayerSkill()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetSphereRadius(15.0f);
	SetRootComponent(Collision);
	
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = 2500.0f;
	ProjectileMovement->MaxSpeed = 2500.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;	// 날아가는 방향으로 회전 True
	ProjectileMovement->bShouldBounce = true;	// 벽에 닿으면 튕김
}

void APlayerSkill::BeginPlay()
{
	Super::BeginPlay();
	
}



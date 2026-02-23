#include "Spawner.h"
#include "Components/BoxComponent.h"

ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawnBox->SetupAttachment(Scene);
	
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();
}

FVector ASpawner::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawnBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawnBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}




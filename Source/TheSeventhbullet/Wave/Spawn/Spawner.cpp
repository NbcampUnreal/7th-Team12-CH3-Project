// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"

#include "Components/BoxComponent.h"
#include "TheSeventhbullet/Manager/SyncDataManager.h"
#include "TheSeventhbullet/System/GameInstance/MainGameInstance.h"


ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawnBox->SetupAttachment(Scene);
	
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawner::InitializeSpawner(const FSpawnRowData& Data)
{
	WaveData = Data;
}

void ASpawner::Spawn()
{
	for (int i = 0 ; i < WaveData.EnemyCount;i++)
	{
		SpawnRandomMonster();
	}
}

AActor* ASpawner::SpawnRandomMonster()
{
	if (WaveData.EnemyTypes.Num() == 0) return nullptr;
	
	int32 RandomIndex = FMath::RandRange(0,WaveData.EnemyTypes.Num()-1);
	
	FName SelectEnemyType = WaveData.EnemyTypes[RandomIndex];
	
	return SpawnMonster(SelectEnemyType);
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

AActor* ASpawner::SpawnMonster(FName MonsterType)
{
	if (MonsterType.IsNone()) return nullptr;
	
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (!GI) return nullptr;
	
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	
	USyncDataManager* DataManager = GI->GetSubsystem<USyncDataManager>();
	if (!DataManager) return nullptr;
	
	const FMonsterRowData MonsterData = DataManager->GetMonsterData(MonsterType);
	
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AActor* SpawnActor = World->SpawnActor<AActor>(
		MonsterData.Monster,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator,
		Params
		);
	return SpawnActor;
}




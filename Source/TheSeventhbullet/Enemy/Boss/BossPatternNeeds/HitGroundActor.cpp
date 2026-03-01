// Fill out your copyright notice in the Description page of Project Settings.


#include "HitGroundActor.h"

#include "Character/MainCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AHitGroundActor::AHitGroundActor()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
}




void AHitGroundActor::Explosion()
{
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);
	for (AActor* HitActor : OverlappingActors)
	{
		AMainCharacter* HittedCharacter=Cast<AMainCharacter>(HitActor);
		if (HittedCharacter==nullptr)
		{
			continue;
		}
		//데미지 처리
			UGameplayStatics::ApplyDamage(
				HittedCharacter,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
			
			//공중에 약간 띄웁니다.
			HittedCharacter->LaunchCharacter(FVector(0,0,LaunchPower),false,true);

	}

	Destroy();
}

void AHitGroundActor::SetDamage(float InDamage)
{
	ExplosionDamage=InDamage;
}


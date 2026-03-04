// Fill out your copyright notice in the Description page of Project Settings.


#include "FireGroundActor.h"

#include "Character/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFireGroundActor::AFireGroundActor()
{
	USceneComponent* SceneComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);
	CollisionComponent=CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(SceneComponent);
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&AFireGroundActor::OnOverlapBegin);
}

void AFireGroundActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* MainCharacter=Cast<AMainCharacter>(OtherActor);
	if (MainCharacter!=nullptr)
	{
		UGameplayStatics::ApplyDamage(
	MainCharacter,
		9999999.f,
nullptr,
this,
		UDamageType::StaticClass()
		);
	}
}



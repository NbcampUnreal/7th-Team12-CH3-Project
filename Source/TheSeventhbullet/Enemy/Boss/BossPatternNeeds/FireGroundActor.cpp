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

	// Overlap 이벤트가 발생하려면 콜리전 프로파일을 OverlapAllDynamic으로 설정해야 함
	// 기본값(BlockAll)이면 Block+Block = Hit만 발생하고 Overlap은 절대 발생하지 않음
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->SetGenerateOverlapEvents(true);

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



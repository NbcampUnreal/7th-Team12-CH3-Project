// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractionActor.h"

#include "Camera/CameraComponent.h"
#include "Character/MainCharacter.h"
#include "Components/BoxComponent.h"

ABaseInteractionActor::ABaseInteractionActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	InteractionComponent = CreateDefaultSubobject<UBaseInteractionComponent>(TEXT("InteractionComponent"));
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	TriggerBox->SetupAttachment(Scene);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Scene);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Scene);
	
	InteractCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InteractCamera"));
	InteractCamera->SetupAttachment(Scene);
}

void ABaseInteractionActor::Interact(AActor* Interactor)
{
	UBaseInteractionComponent* InteractComp = FindComponentByClass<UBaseInteractionComponent>();
	
	if (InteractComp)
	{
		InteractComp->BeginInteract(Interactor);
	}
}

void ABaseInteractionActor::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseInteractionActor::OnOverlapBegin);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABaseInteractionActor::OnOverlapEnd);
	}
}

void ABaseInteractionActor::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if(Player)
	{
		//TODO : 플레이어의 CurrentInteractableTarget(임시로 만든 변수명) 변수에 자기 자신(this),즉 이 BaseInterationActor 등록
		//TODO : 플레이어 화면에 "F키를 눌러 상호작용" UI를 띄움
	}
}

void ABaseInteractionActor::OnOverlapEnd(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if(Player)
	{
		//TODO : 플레이어의 CurrentInteractableTarget(임시로 만든 변수명) 변수를 nullptr로 비워줌
		//TODO : 플레이어 화면에 "F키를 눌러 상호작용" UI를 숨김
	}
}


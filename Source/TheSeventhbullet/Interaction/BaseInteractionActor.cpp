// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractionActor.h"

#include "Camera/CameraComponent.h"
#include "Character/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/InteractionPromptWidget.h"

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

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(Scene);
	InteractionWidget->SetRelativeLocation(PromptOffset);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawAtDesiredSize(true);
	InteractionWidget->SetVisibility(false);
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

	if (InteractionWidget && PromptWidgetClass)
	{
		InteractionWidget->SetWidgetClass(PromptWidgetClass);

		if (UInteractionPromptWidget* PromptWidget = Cast<UInteractionPromptWidget>(InteractionWidget->GetUserWidgetObject()))
		{
			PromptWidget->SetPromptText(PromptText);
		}
	}
}

void ABaseInteractionActor::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->SetCurrentInteractable(this);
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(true);
		}
	}
}

void ABaseInteractionActor::OnOverlapEnd(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->SetCurrentInteractable(nullptr);
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(false);
		}
	}
}


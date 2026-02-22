// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractionComponent.h"

#include "Kismet/GameplayStatics.h"

UBaseInteractionComponent::UBaseInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UBaseInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBaseInteractionComponent::BeginInteract(AActor* Interactor)
{
	APawn* PlayerPawn = Cast<APawn>(Interactor);
	if (PlayerPawn)
	{
		APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
		if (PC)
		{
			PC->SetViewTargetWithBlend(GetOwner(),0.5f);
		}
	}
	ProgressInteract(Interactor);
}

void UBaseInteractionComponent::ProgressInteract(AActor* Interactor)
{
}

void UBaseInteractionComponent::EndInteract(AActor* Interactor)
{
	APawn* PlayerPawn = Cast<APawn>(Interactor);
	if (PlayerPawn == nullptr)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(this,0);
	}
	
	APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
	if (PC)
	{
		PC->SetViewTargetWithBlend(PlayerPawn,0.5f);
	}
}



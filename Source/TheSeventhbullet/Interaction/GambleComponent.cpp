// Fill out your copyright notice in the Description page of Project Settings.


#include "GambleComponent.h"

#include "Kismet/GameplayStatics.h"

UGambleComponent::UGambleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UGambleComponent::BeginPlay()
{
	Super::BeginPlay();
	// AActor* TestPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	//
	// if (TestPlayer)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("테스트: BeginPlay에서 도박을 강제 실행합니다."));
	// 	BeginInteract(TestPlayer);
	// 	EndInteract(TestPlayer);
	// }
}

void UGambleComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);
	
}

void UGambleComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
	bool bWin = true;
	//TODO : 도박장 로직 구현
	
	OnInteractionResult.Broadcast(bWin);
	
}

void UGambleComponent::EndInteract(AActor* Interactor)
{
	
	Super::EndInteract(Interactor);
}





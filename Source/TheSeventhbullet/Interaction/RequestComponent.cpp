#include "RequestComponent.h"

URequestComponent::URequestComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void URequestComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);
}

void URequestComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
}

void URequestComponent::EndInteract(AActor* Interactor)
{
	Super::EndInteract(Interactor);
}


void URequestComponent::BeginPlay()
{
	Super::BeginPlay();
	
}




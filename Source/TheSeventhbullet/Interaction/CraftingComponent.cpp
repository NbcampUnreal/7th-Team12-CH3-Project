#include "CraftingComponent.h"

UCraftingComponent::UCraftingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCraftingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCraftingComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);
}

void UCraftingComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
}

void UCraftingComponent::EndInteract(AActor* Interactor)
{
	Super::EndInteract(Interactor);
}

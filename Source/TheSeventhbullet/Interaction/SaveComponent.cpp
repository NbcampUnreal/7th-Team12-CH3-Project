#include "SaveComponent.h"

#include "System/GameInstance/MainGameInstance.h"

USaveComponent::USaveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaveComponent::BeginPlay()
{
	Super::BeginPlay();
	GI = UMainGameInstance::Get(this);
}

void USaveComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);
	ProgressInteract(Interactor);
}

void USaveComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
	if (!GI) return;
	GI->SaveGameData();
}

void USaveComponent::EndInteract(AActor* Interactor)
{
	if (!GI) return;
	
	GI->CurrentDay++;
	
	Super::EndInteract(Interactor);
}



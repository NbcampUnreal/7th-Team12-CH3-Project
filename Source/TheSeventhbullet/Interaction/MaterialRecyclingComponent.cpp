#include "MaterialRecyclingComponent.h"

UMaterialRecyclingComponent::UMaterialRecyclingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMaterialRecyclingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMaterialRecyclingComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);
}

void UMaterialRecyclingComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
}

void UMaterialRecyclingComponent::EndInteract(AActor* Interactor)
{
	Super::EndInteract(Interactor);
}

bool UMaterialRecyclingComponent::AddMaterial(UMaterialDataAsset* Material)
{
	if (!Material)
	{
		return false;
	}
	if (RecyclingMaterials.Num() >= RecyclingCount)
	{
		return false;
	}
	
	RecyclingMaterials.Add(Material);
	BroadcastChanged();
	return true;
}

bool UMaterialRecyclingComponent::RemoveMaterial(int32 Index)
{
	if (!RecyclingMaterials.IsValidIndex(Index))
	{
		return false;
	}
	
	RecyclingMaterials.RemoveAt(Index);
	BroadcastChanged();
	return true;
}

void UMaterialRecyclingComponent::ClearMaterials()
{
	RecyclingMaterials.Reset();
	BroadcastChanged();
}

bool UMaterialRecyclingComponent::RecycleMaterials(UMaterialDataAsset* Material)
{
	return true;
}

void UMaterialRecyclingComponent::BroadcastChanged() const
{
	TArray<UMaterialDataAsset*> Materials;
	for (UMaterialDataAsset* Mat : RecyclingMaterials)
	{
		Materials.Add(Mat);
	}
	OnRecyclingMaterialsChanged.Broadcast(Materials);
}

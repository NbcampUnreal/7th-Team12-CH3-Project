#include "CraftingTableActor.h"
#include "CraftingFunctionLibrary.h"

ACraftingTableActor::ACraftingTableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ACraftingTableActor::AddMaterial(UMaterialDataAsset* Material)
{
	if (!Material)
	{
		return false;
	}
	if (SourceMaterials.Num() >= MaxMaterialCount)
	{
		return false;
	}
	
	SourceMaterials.Add(Material);
	BroadcastChanged();
	return true;
}

bool ACraftingTableActor::RemoveMaterial(int32 Index)
{
	if (!SourceMaterials.IsValidIndex(Index))
	{
		return false;
	}
	
	SourceMaterials.RemoveAt(Index);
	BroadcastChanged();
	return true;
}

void ACraftingTableActor::ClearMaterials()
{
	SourceMaterials.Reset();
	BroadcastChanged();
}

bool ACraftingTableActor::CraftSoulGem(FSoulGemInstance& ResultSoulGem)
{
	TArray<UMaterialDataAsset*> Materials;
	for (UMaterialDataAsset* Mat : SourceMaterials)
	{
		Materials.Add(Mat);
	}
	
	if (Materials.Num() < MinMaterialCount)
	{
		return false;
	}
	
	if (!UCraftingFunctionLibrary::CraftSoulGemFromMaterial(Materials, ResultSoulGem))
	{
		return false;
	}
	
	OnSoulGemCrafted.Broadcast(ResultSoulGem);
	ClearMaterials();
	return true;
}

void ACraftingTableActor::BroadcastChanged() const
{
	TArray<UMaterialDataAsset*> Materials;
	for (UMaterialDataAsset* Mat : SourceMaterials)
	{
		Materials.Add(Mat);
	}
	OnCraftMaterialsChanged.Broadcast(Materials);
}

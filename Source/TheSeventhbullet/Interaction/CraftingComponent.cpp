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

bool UCraftingComponent::AddMaterial(UMaterialDataAsset* Material)
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

bool UCraftingComponent::RemoveMaterial(int32 Index)
{
	if (!SourceMaterials.IsValidIndex(Index))
	{
		return false;
	}
	
	SourceMaterials.RemoveAt(Index);
	BroadcastChanged();
	return true;
}

void UCraftingComponent::ClearMaterials()
{
	SourceMaterials.Reset();
	BroadcastChanged();
}

bool UCraftingComponent::CraftSoulGem(FSoulGemInstance& ResultSoulGem)
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
	
	if (!CraftSoulGemFromMaterial(Materials, ResultSoulGem))
	{
		return false;
	}
	
	OnSoulGemCrafted.Broadcast(ResultSoulGem);
	ClearMaterials();
	return true;
}

bool UCraftingComponent::CraftSoulGemFromMaterial(const TArray<UMaterialDataAsset*>& Materials,
	FSoulGemInstance& ResultSoulGem)
{
	ResultSoulGem = FSoulGemInstance();
	
	ResultSoulGem.GemName = CraftSoulGemName(Materials);
	MergeEffects(Materials, ResultSoulGem);
	return true;
}

FText UCraftingComponent::CraftSoulGemName(const TArray<UMaterialDataAsset*>& Materials)
{
	// 이름 규칙
	// 1번 Material : 이름에 영향없음
	// 2번 Material : Prefix
	// 3번 Material : Suffix
	// 4번 Material : GemBaseName
	FText Prefix = FText::GetEmpty();
	FText Suffix = FText::GetEmpty();
	FText GemBaseName = FText::GetEmpty();
	
	if (Materials.Num() >= 2 && Materials[1])
	{
		Prefix = Materials[1] -> NamingParts.Prefix;
	}
	if (Materials.Num() >= 3 && Materials[2])
	{
		Suffix = Materials[2] -> NamingParts.Suffix;
	}
	if (Materials.Num() >= 4 && Materials[3])
	{
		GemBaseName = Materials[3] -> NamingParts.GemBaseName;
	}
	
	FText CombineName = FText::Format(NSLOCTEXT("SoulGem", "NameFormat", "{0} {1} {2}"), Prefix, Suffix, GemBaseName);
	
	return CombineName;
}

void UCraftingComponent::MergeEffects(const TArray<UMaterialDataAsset*>& Materials, FSoulGemInstance& ResultSoulGem)
{
	int32 TempGrade = 0;
	
	for (UMaterialDataAsset* Mat : Materials)
	{
		if (!Mat) continue;
		
		ResultSoulGem.StatusModifiers.Add(Mat->StatusModifier);
		TempGrade += Mat->Grade;
		
		if (Mat->SpecialOption != ESpecialOptions::None)
		{
			ResultSoulGem.SpecialOptions.Add(Mat->SpecialOption);
		}
		
		// 합성한 재료 이름 저장.
		ResultSoulGem.SourceMaterialNames.Add(Mat->GetFName());
	}	
	
	ResultSoulGem.Grade = TempGrade / Materials.Num();
}

void UCraftingComponent::BroadcastChanged() const
{
	TArray<UMaterialDataAsset*> Materials;
	for (UMaterialDataAsset* Mat : SourceMaterials)
	{
		Materials.Add(Mat);
	}
	OnCraftMaterialsChanged.Broadcast(Materials);
}

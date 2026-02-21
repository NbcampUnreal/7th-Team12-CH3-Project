#pragma once

#include "CoreMinimal.h"
#include "DataAsset/MaterialDataAsset.h"
#include "SoulGem/SoulGemProperties.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CraftingFunctionLibrary.generated.h"

UCLASS()
class THESEVENTHBULLET_API UCraftingFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	static bool CraftSoulGemFromMaterial(const TArray<UMaterialDataAsset*>& Materials, FSoulGemProperties& ResultSoulGem);
	
private:
	static FText CraftSoulGemName(const TArray<UMaterialDataAsset*>& Materials);
	static void MergeEffects(const TArray<UMaterialDataAsset*>& Materials, FSoulGemProperties& ResultSoulGem);
};

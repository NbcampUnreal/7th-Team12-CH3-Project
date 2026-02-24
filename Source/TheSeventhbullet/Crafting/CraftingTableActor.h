#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAsset/MaterialDataAsset.h"
#include "SoulGem/SoulGemInstance.h"
#include "CraftingTableActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftMaterialsChanged, const TArray<UMaterialDataAsset*>&, Materials);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulGemCrafted, const FSoulGemInstance&, Result);

UCLASS()
class THESEVENTHBULLET_API ACraftingTableActor : public AActor
{
	GENERATED_BODY()

public:
	ACraftingTableActor();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	int32 MaxMaterialCount = 4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	int32 MinMaterialCount = 3;
	
	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnCraftMaterialsChanged OnCraftMaterialsChanged;
	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnSoulGemCrafted OnSoulGemCrafted;
	
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool AddMaterial(UMaterialDataAsset* Material);
	
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool RemoveMaterial(int32 Index);
	
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void ClearMaterials();
	
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CraftSoulGem(FSoulGemInstance& ResultSoulGem);
	
	const TArray<UMaterialDataAsset*>& GetSourceMaterials() const { return SourceMaterials; }
	
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	const FText GetGemName(const FSoulGemInstance& ResultSoulGem) const { return ResultSoulGem.GemName; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	TArray<TObjectPtr<UMaterialDataAsset>> SourceMaterials;
	
private:
	void BroadcastChanged() const;
	
	
};

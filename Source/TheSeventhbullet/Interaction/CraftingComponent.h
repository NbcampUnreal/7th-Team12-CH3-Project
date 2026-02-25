#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "CraftingComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftMaterialsChanged, const TArray<UMaterialDataAsset*>&, Materials);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulGemCrafted, const FSoulGemInstance&, Result);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UCraftingComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	UCraftingComponent();

protected:
	virtual void BeginPlay() override;
public:
	virtual void BeginInteract(AActor* Interactor) override;
	virtual void ProgressInteract(AActor* Interactor) override;
	virtual void EndInteract(AActor* Interactor) override;
	
public:
	//UPROPERTY(BlueprintAssignable, Category = "Crafting")
	// OnCraftMaterialsChanged;
	//UPROPERTY(BlueprintAssignable, Category = "Crafting")
	//FOnSoulGemCrafted OnSoulGemCrafted;
};

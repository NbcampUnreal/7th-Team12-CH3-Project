#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "DataAsset/MaterialDataAsset.h"
#include "SoulGem/SoulGemInstance.h"
#include "CraftingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftMaterialsChanged, const TArray<UMaterialDataAsset*>&, Materials);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulGemCrafted, const FSoulGemInstance&, Result);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UCraftingComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void BeginInteract(AActor* Interactor) override;
	virtual void ProgressInteract(AActor* Interactor) override;
	virtual void EndInteract(AActor* Interactor) override;
	
public:
	UCraftingComponent();
	
	// 재료를 SourceMaterials 배열에 추가하는 함수.
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	bool AddMaterial(UMaterialDataAsset* Material);
	// 재료를 SourceMaterials 배열에서 빼는 함수.
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	bool RemoveMaterial(int32 Index);
	// SourceMaterials 배열에 올라간 재료를 전부 Clear하는 함수.
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	void ClearMaterials();
	
	// 제작 버튼을 누르면 실행하는 함수. 제작으로 만들어진 소울젬을 OnSoulGemCrafted로 알려줌
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	bool CraftSoulGem(FSoulGemInstance& ResultSoulGem);
	// 실제 재료들을 가지고 실제로 소울젬의 이름과 효과를 정하는 함수.
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	bool CraftSoulGemFromMaterial(const TArray<UMaterialDataAsset*>& Materials, FSoulGemInstance& ResultSoulGem);
	// 재료들의 이름을 규칙에 따라 합치는 합수.
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	FText CraftSoulGemName(const TArray<UMaterialDataAsset*>& Materials);
	// 재료들의 효과를 하나로 합치는 함수
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	void MergeEffects(const TArray<UMaterialDataAsset*>& Materials, FSoulGemInstance& ResultSoulGem);
	
	// 재료가 들어가고 빠질 때마다 변화를 OnCraftMaterialsChanged로 현재 SourceMaterials 배열에 올라가 있는 재료들을 알려줌.
	void BroadcastChanged() const;

public:
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	const TArray<UMaterialDataAsset*>& GetSourceMaterials() const { return SourceMaterials; }
	UFUNCTION(BlueprintCallable, Category = "CraftingComp")
	const FText GetGemName(const FSoulGemInstance& ResultSoulGem) const { return ResultSoulGem.GemName; }
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnCraftMaterialsChanged OnCraftMaterialsChanged;
	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnSoulGemCrafted OnSoulGemCrafted;
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "CraftingComp", meta = (AllowPrivateAccess = "true"))
	int32 MaxMaterialCount = 4; // 최대 합성 재료 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CraftingComp", meta = (AllowPrivateAccess = "true"))
	int32 MinMaterialCount = 3; // 최소 합성 재료 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UMaterialDataAsset>> SourceMaterials; // 합성에 들어갈 재료들
};

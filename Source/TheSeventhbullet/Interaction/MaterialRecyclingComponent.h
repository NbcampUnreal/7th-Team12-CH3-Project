#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "DataAsset/MaterialDataAsset.h"
#include "MaterialRecyclingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecyclingMaterialsChanged, const TArray<UMaterialDataAsset*>&, Materials);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaterialRecycled, const FSoulGemInstance&, Result);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UMaterialRecyclingComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	UMaterialRecyclingComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void BeginInteract(AActor* Interactor) override;
	virtual void ProgressInteract(AActor* Interactor) override;
	virtual void EndInteract(AActor* Interactor) override;
	
	// 재료를 RecyclingMaterials 배열에 추가하는 함수.
	UFUNCTION(BlueprintCallable, Category = "RecyclingComp")
	bool AddMaterial(UMaterialDataAsset* Material);
	// 재료를 RecyclingMaterials 배열에서 빼는 함수.
	UFUNCTION(BlueprintCallable, Category = "RecyclingComp")
	bool RemoveMaterial(int32 Index);
	// RecyclingMaterials 배열에 올라간 재료를 전부 Clear하는 함수.
	UFUNCTION(BlueprintCallable, Category = "RecyclingComp")
	void ClearMaterials();
	
	// 재활용(예시) 버튼을 누르면 실행하는 함수. 재활용으로 만들어진 Material을 OnMaterialRecycled로 알려줌
	UFUNCTION(BlueprintCallable, Category = "RecyclingComp")
	bool RecycleMaterials(UMaterialDataAsset* Material);
	
	// 재료가 들어가고 빠질 때마다 변화를 OnRecyclingMaterialsChanged로 현재 RecyclingMaterials 배열에 올라가 있는 재료들을 알려줌.
	void BroadcastChanged() const;
	
public:
	// DELEGATE METHOD
	UPROPERTY(BlueprintAssignable, Category = "RecyclingComp")
	FOnRecyclingMaterialsChanged OnRecyclingMaterialsChanged;
	UPROPERTY(BlueprintAssignable, Category = "RecyclingComp")
	FOnMaterialRecycled OnMaterialRecycled;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RecyclingComp", meta = (AllowPrivateAccess = "true"))
	int32 RecyclingCount = 3; // 재활용 재료 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RecyclingComp", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UMaterialDataAsset>> RecyclingMaterials; // 재활용에 들어갈 재료들
};

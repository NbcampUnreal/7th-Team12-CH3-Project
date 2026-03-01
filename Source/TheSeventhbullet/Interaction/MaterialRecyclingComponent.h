#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "DataAsset/MaterialDataAsset.h"
#include "MaterialRecyclingComponent.generated.h"

struct FItemInstance;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecyclingMaterialsChanged, const TArray<UMaterialDataAsset*>&, Materials);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaterialRecycled, const FItemInstance&, Result);

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
	bool RecycleMaterials(FItemInstance& ResultMaterial);
	
	// 재료가 들어가고 빠질 때마다 변화를 OnRecyclingMaterialsChanged로 현재 RecyclingMaterials 배열에 올라가 있는 재료들을 알려줌.
	void BroadcastChanged() const;
	
private:
	int32 CalculateAvgGrade() const;
	int32 RollResultGrade(int32 Grade) const;
	bool PickFromPool(const TArray<struct FMaterialDropEntry>& RecyclingPool,
	                  TSoftObjectPtr<UMaterialDataAsset>& PickedMaterial) const;
	
public:
	// DELEGATE METHOD
	UPROPERTY(BlueprintAssignable, Category = "RecyclingComp")
	FOnRecyclingMaterialsChanged OnRecyclingMaterialsChanged;
	UPROPERTY(BlueprintAssignable, Category = "RecyclingComp")
	FOnMaterialRecycled OnMaterialRecycled;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RecyclingComp", meta = (AllowPrivateAccess = "true"))
	int32 RecyclingMaterialCount = 3; // 재활용 재료 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RecyclingComp", meta = (AllowPrivateAccess = "true"))
	float PlusChance = 0.3f; // 결과물로 나오는 Material의 Grade가 1단계 상승할 확률.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RecyclingComp", meta = (AllowPrivateAccess = "true"))
	int32 MaxGrade = 6; // 결과물로 나올 수 있는 Grade의 최대치. Material의 단계가 상승하더라도 7단계가 나올 수는 없어야 하니까
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RecyclingComp", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UMaterialDataAsset>> RecyclingMaterials; // 재활용에 들어갈 재료들
};

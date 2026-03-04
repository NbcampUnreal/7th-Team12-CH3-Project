#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "Data/TableRowTypes.h"
#include "RequestComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API URequestComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	URequestComponent();

	UFUNCTION(BlueprintCallable, Category = "Interaction|Request")
	void SelectRequest(int32 SelectedRequestID);

	virtual void BeginInteract(AActor* Interactor) override;
	virtual void ProgressInteract(AActor* Interactor) override;
	virtual void EndInteract(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

	// 날짜별 등장 가능한 레벨
	TArray<int32> DayAvailableLevel = {1,2,2,3,3,3};

private:
	// 의뢰가 마지막으로 갱신된 날짜 (-1이면 미갱신)
	int32 LastRefreshedDay = -1;

	// 캐시된 의뢰 데이터
	TArray<int32> CachedRequestIDs;
	TArray<FRequestRowData> CachedRequests;

	// 의뢰 목록 갱신
	void RefreshRequests();
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/TableRowTypes.h"
#include "RequestWidget.generated.h"

class UButton;
class UVerticalBox;
class URequestSlotWidget;
class URequestComponent;

/**
 * 의뢰 게시판 위젯 - 의뢰 목록 표시 + 선택
 *
 * [UMG 필수 바인딩]
 *  - SlotContainer : 의뢰 슬롯이 들어갈 VerticalBox
 *  - CloseButton   : 닫기 버튼
 *
 * [BP 설정]
 *  - SlotWidgetClass : RequestSlotWidget의 BP 서브클래스 지정
 */
UCLASS()
class THESEVENTHBULLET_API URequestWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRequests(URequestComponent* InRequestComp,
		const TArray<int32>& RequestIDs,
		const TArray<FRequestRowData>& Requests);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Request")
	TSubclassOf<URequestSlotWidget> SlotWidgetClass;

private:
	UFUNCTION()
	void OnCloseClicked();

	void OnSlotAccepted(int32 RequestID);

	UPROPERTY()
	TObjectPtr<URequestComponent> RequestComp;

	UPROPERTY()
	TArray<TObjectPtr<URequestSlotWidget>> SlotWidgets;
};

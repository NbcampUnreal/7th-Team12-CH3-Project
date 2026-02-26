#include "RequestWidget.h"

#include "RequestSlotWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Interaction/RequestComponent.h"

void URequestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &URequestWidget::OnCloseClicked);
	}
}

void URequestWidget::SetRequests(URequestComponent* InRequestComp,
	const TArray<int32>& RequestIDs,
	const TArray<FRequestRowData>& Requests)
{
	RequestComp = InRequestComp;

	if (!SlotContainer || !SlotWidgetClass) return;

	// 기존 슬롯 제거
	for (URequestSlotWidget* RequestSlot : SlotWidgets)
	{
		if (RequestSlot)
		{
			RequestSlot->RemoveFromParent();
		}
	}
	SlotWidgets.Empty();

	// 새 슬롯 생성
	for (int32 i = 0; i < Requests.Num(); ++i)
	{
		URequestSlotWidget* RequestSlot = CreateWidget<URequestSlotWidget>(this, SlotWidgetClass);
		if (!RequestSlot) continue;

		RequestSlot->SetRequestInfo(RequestIDs[i], Requests[i]);
		RequestSlot->OnRequestAccepted.BindUObject(this, &URequestWidget::OnSlotAccepted);

		SlotContainer->AddChild(RequestSlot);
		SlotWidgets.Add(RequestSlot);
	}
}

void URequestWidget::OnCloseClicked()
{
	if (RequestComp)
	{
		RequestComp->EndInteract(nullptr);
	}
}

void URequestWidget::OnSlotAccepted(int32 RequestID)
{
	if (RequestComp)
	{
		RequestComp->SelectRequest(RequestID);
	}
}

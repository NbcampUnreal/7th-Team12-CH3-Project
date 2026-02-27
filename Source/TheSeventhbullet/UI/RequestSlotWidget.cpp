#include "RequestSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void URequestSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &URequestSlotWidget::OnAcceptClicked);
	}
}

void URequestSlotWidget::SetRequestInfo(int32 InRequestID, const FRequestRowData& Data)
{
	RequestID = InRequestID;

	if (RequestNameText)
	{
		RequestNameText->SetText(FText::FromName(Data.RequestName));
	}
	if (GoldText)
	{
		GoldText->SetText(FText::AsNumber(Data.RequestGold));
	}
	if (LevelText)
	{
		LevelText->SetText(FText::Format(
			FText::FromString(TEXT("Lv.{0}")), FText::AsNumber(Data.RequestLevel)));
	}
	if (WaveCountText)
	{
		WaveCountText->SetText(FText::Format(
			FText::FromString(TEXT("{0} Waves")), FText::AsNumber(Data.Waves.Num())));
	}
}

void URequestSlotWidget::OnAcceptClicked()
{
	OnRequestAccepted.ExecuteIfBound(RequestID);
}

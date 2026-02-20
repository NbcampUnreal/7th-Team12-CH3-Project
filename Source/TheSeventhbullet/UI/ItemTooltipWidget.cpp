#include "ItemTooltipWidget.h"
#include "Components/TextBlock.h"

void UItemTooltipWidget::SetItemInfo(const FText& Name, const FText& Desc)
{
	if (ItemNameText)
	{
		ItemNameText->SetText(Name);
	}
	if (ItemDescText)
	{
		ItemDescText->SetText(Desc);
	}
}

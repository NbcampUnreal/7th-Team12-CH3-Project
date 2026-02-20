#include "ItemTooltipWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UItemTooltipWidget::SetItemInfo(const FText& Name, const FText& Desc, int32 Count, UTexture2D* Icon)
{
	if (ItemNameText)
	{
		ItemNameText->SetText(Name);
	}
	if (ItemDescText)
	{
		ItemDescText->SetText(Desc);
	}
	if (ItemCountText)
	{
		ItemCountText->SetText(FText::Format(NSLOCTEXT("Inventory", "CountFormat", "x{0}"), Count));
	}
	if (ItemIconImage)
	{
		if (Icon)
		{
			ItemIconImage->SetBrushFromTexture(Icon);
			ItemIconImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemIconImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
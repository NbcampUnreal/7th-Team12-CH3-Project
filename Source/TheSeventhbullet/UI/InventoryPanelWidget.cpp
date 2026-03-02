#include "InventoryPanelWidget.h"
#include "CharacterInfoWidget.h"
#include "InventoryWidget.h"

void UInventoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryPanelWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (InVisibility == ESlateVisibility::Visible && CharacterInfoWidget)
	{
		CharacterInfoWidget->RefreshInfo();
	}
}
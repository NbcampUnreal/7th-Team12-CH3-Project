#include "DamageNumberWidget.h"
#include "Components/TextBlock.h"

void UDamageNumberWidget::SetDamageInfo(float Damage, bool bIsCrit)
{
	if (!DamageText) return;

	DamageText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(Damage))));

	if (bIsCrit)
	{
		DamageText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));

		FSlateFontInfo FontInfo = DamageText->GetFont();
		FontInfo.Size = 32;
		DamageText->SetFont(FontInfo);
	}
	else
	{
		DamageText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		FSlateFontInfo FontInfo = DamageText->GetFont();
		FontInfo.Size = 32;
		DamageText->SetFont(FontInfo);
	}
}

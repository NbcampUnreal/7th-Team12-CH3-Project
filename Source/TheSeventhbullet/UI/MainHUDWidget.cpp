#include "MainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateHP(1.f, 1.f);
	UpdateAmmo(0, 0);
}

//스탯관리하는 부분에서 BroadCast받아 올 예정
void UMainHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	if (HPBar)
	{
		float Ratio = (MaxHP > 0.f) ? FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f) : 0.f;
		HPBar->SetPercent(Ratio);
	}
}

//스탯관리하는 부분에서 BroadCast받아 올 예정
void UMainHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
	}
}
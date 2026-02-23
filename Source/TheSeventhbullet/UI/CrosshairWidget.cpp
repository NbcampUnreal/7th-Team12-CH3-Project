#include "CrosshairWidget.h"
#include "Components/Image.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetCrosshairVisible(true);
	HideHitMarker();
}

void UCrosshairWidget::SetCrosshairVisible(bool bVisible)
{
	if (CrosshairImage)
	{
		CrosshairImage->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

// ApplyDamageByHit에서 BroadCast 받을 예정
void UCrosshairWidget::ShowHitMarker()
{
	if (!HitMarkerImage) return;

	HitMarkerImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitMarkerTimerHandle);
		World->GetTimerManager().SetTimer(
			HitMarkerTimerHandle,
			FTimerDelegate::CreateUObject(this, &UCrosshairWidget::HideHitMarker),
			HitMarkerDuration,
			false
		);
	}
}

void UCrosshairWidget::HideHitMarker()
{
	if (HitMarkerImage)
	{
		HitMarkerImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}
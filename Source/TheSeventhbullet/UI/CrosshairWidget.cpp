#include "CrosshairWidget.h"
#include "Components/Image.h"
#include "Character/MainCharacter.h"
#include "Character/Component/CombatComponent.h"
#include "Kismet/GameplayStatics.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetCrosshairVisible(true);
	HideHitMarker();

	if (AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UCombatComponent* Combat = Character->CombatComponent)
		{
			Combat->OnHitConfirm.AddDynamic(this, &UCrosshairWidget::ShowHitMarker);
		}
	}
}

void UCrosshairWidget::NativeDestruct()
{
	if (AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UCombatComponent* Combat = Character->CombatComponent)
		{
			Combat->OnHitConfirm.RemoveDynamic(this, &UCrosshairWidget::ShowHitMarker);
		}
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitMarkerTimerHandle);
	}

	Super::NativeDestruct();
}

void UCrosshairWidget::SetCrosshairVisible(bool bVisible)
{
	if (CrosshairImage)
	{
		CrosshairImage->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UCrosshairWidget::ShowHitMarker(bool bIsHeadShot)
{
	if (!HitMarkerImage) return;

	HitMarkerImage->SetColorAndOpacity(bIsHeadShot ? HeadShotColor : NormalHitColor);
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

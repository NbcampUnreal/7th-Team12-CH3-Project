// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void USaveWidget::PlaySleepAnimation()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	SetOverlayAlpha(0.f);
	
	if (MoonImage)
	{
		MoveCelestial(MoonImage, 0.f,false);
		MoonImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		MoonImage->SetRenderOpacity(1.f);
	}
	if (SunImage)
		SunImage->SetVisibility(ESlateVisibility::Hidden);
	SetPhase(EPhase::Sleep_Travel);
}

void USaveWidget::PlayWakeAnimation()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	SetOverlayAlpha(1.f);
	
	if (SunImage)
	{
		MoveCelestial(SunImage, 0.f,true);
		SunImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		SunImage->SetRenderOpacity(1.f);
	}
	if (MoonImage)
		MoonImage->SetVisibility(ESlateVisibility::Hidden);
	SetPhase(EPhase::Wake_FadeIn);
}

void USaveWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ArchImage && ArchTexture) ArchImage->SetBrushFromTexture(ArchTexture);
	if (MoonImage && MoonTexture) MoonImage->SetBrushFromTexture(MoonTexture);
	if (SunImage  && SunTexture) SunImage->SetBrushFromTexture(SunTexture);
	
	SetOverlayAlpha(0.f);
	if (MoonImage)
		MoonImage->SetVisibility(ESlateVisibility::Hidden);
	if (SunImage)
		SunImage->SetVisibility(ESlateVisibility::Hidden);
	SetVisibility(ESlateVisibility::Hidden);
}

void USaveWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (Phase == EPhase::Idle) return;
	
	Elapsed += InDeltaTime;

	switch (Phase)
	{
	case EPhase::Sleep_Travel:
		TickSleepTravel(InDeltaTime);
		break;
	case EPhase::Sleep_FadeOut:
		TickSleepFade(InDeltaTime);
		break;
	case EPhase::Wake_FadeIn:
		TickWakeFade(InDeltaTime);
		break;
	case EPhase::Wake_Travel:
		TickWakeTravel(InDeltaTime);
		break;
	default:
		break;
	}
}

void USaveWidget::SetPhase(EPhase NewPhase)
{
	Phase = NewPhase;
	Elapsed = 0.f;
}

void USaveWidget::TickSleepTravel(float Dt)
{
	const float T = FMath::Clamp(Elapsed / TravelDuration, 0.f, 1.f);
	MoveCelestial(MoonImage, EaseInOut(T),false);
	
	if (T > 0.5f)
		SetOverlayAlpha((T-0.5f) / 0.5f * 0.5f);
	
	if ( T>= 1.f)
	{
		if (MoonImage)
			MoonImage->SetVisibility(ESlateVisibility::Hidden);
		SetPhase(EPhase::Sleep_FadeOut);
	}
}

void USaveWidget::TickSleepFade(float Dt)
{
	const float T = FMath::Clamp(Elapsed / FadeDuration, 0.f, 1.f);
	SetOverlayAlpha(FMath::Lerp(0.5f,1.f,EaseInOut(T)));
	
	if (T >= 1.f)
	{
		SetOverlayAlpha(1.f);
		SetPhase(EPhase::Idle);
		OnTransitionComplete.Broadcast();
	}
}

void USaveWidget::TickWakeFade(float Dt)
{
	const float T = FMath::Clamp(Elapsed / FadeDuration, 0.f,1.f);
	SetOverlayAlpha(FMath::Lerp(1.f, 0.f, EaseInOut(T)));
	
	if (T >= 1.f)
	{
		SetOverlayAlpha(0.f);
		SetPhase(EPhase::Wake_Travel);
	}
}

void USaveWidget::TickWakeTravel(float Dt)
{
	const float T = FMath::Clamp(Elapsed / TravelDuration, 0.f, 1.f);
	MoveCelestial(SunImage, EaseInOut(T),true);
	
	if (T >= 1.f)
	{
		if (SunImage)
			SunImage->SetVisibility(ESlateVisibility::Hidden);
		SetVisibility(ESlateVisibility::Hidden);
		SetPhase(EPhase::Idle);
		OnTransitionComplete.Broadcast();
	}
}

void USaveWidget::MoveCelestial(UImage* Img, float Progress, bool bReverse) const
{
	if (!Img) return;

	UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Img->Slot);
	if (!Slot) return;

	FVector2D ViewportSize(1920.f, 1080.f);
	if (GEngine && GEngine->GameViewport)
		GEngine->GameViewport->GetViewportSize(ViewportSize);

	const FVector2D ImgSize = Slot->GetSize();

	// bReverse=false : 왼쪽 밖(-ImgW) → 오른쪽 밖(ViewportW)
	// bReverse=true  : 오른쪽 밖(ViewportW) → 왼쪽 밖(-ImgW)
	const float StartX = bReverse ?  ViewportSize.X          : -ImgSize.X;
	const float EndX   = bReverse ? -ImgSize.X               :  ViewportSize.X;
	const float PosX   = FMath::Lerp(StartX, EndX, Progress);
	const float PosY   = ViewportSize.Y * PathYRatio - ImgSize.Y * 0.5f;

	Slot->SetPosition(FVector2D(PosX, PosY));
}
void USaveWidget::SetOverlayAlpha(float Alpha)
{
	if (BackgroundOverlay)
		BackgroundOverlay->SetRenderOpacity(FMath::Clamp(Alpha,0.f,1.f));
}

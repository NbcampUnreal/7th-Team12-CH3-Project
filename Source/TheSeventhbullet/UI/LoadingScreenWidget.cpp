#include "LoadingScreenWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetProgress(0.0f);
	PlayAnimation(SpinAnimation, 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);
}

void ULoadingScreenWidget::SetProgress(float Percent)
{
	Percent = FMath::Clamp(Percent, 0.0f, 1.0f);

	if (LoadingBar)
	{
		LoadingBar->SetPercent(Percent);
	}

	if (LoadingText)
	{
		int32 DisplayPercent = FMath::RoundToInt(Percent * 100.0f);
		LoadingText->SetText(FText::FromString(FString::Printf(TEXT("Loading... %d %%"), DisplayPercent)));
	}
}

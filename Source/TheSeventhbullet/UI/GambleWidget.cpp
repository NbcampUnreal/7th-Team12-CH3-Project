#include "UI/GambleWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"

void UGambleWidget::StartGamble(const FGambleResultData& InData)
{
	CachedData = InData;
	CurrentGemIndex = 0;
	DisplayedScore = 0;
	
	if (ResultText)
		ResultText->SetVisibility(ESlateVisibility::Hidden);
	if (ResultImage)
		ResultImage->SetVisibility(ESlateVisibility::Hidden);
	
	SetPhase(EGamblePhase::Init);
}

void UGambleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ResultText)
		ResultText->SetVisibility(ESlateVisibility::Hidden);
	if (ResultImage)
		ResultImage->SetVisibility(ESlateVisibility::Hidden);
}

void UGambleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (Phase == EGamblePhase::Idle) return;
	Elapsed += InDeltaTime;
	switch (Phase)
	{
	case EGamblePhase::Init:		TickInit();			break;
	case EGamblePhase::GemReveal:	TickGemReveal();	break;
	case EGamblePhase::ScoreCount:	TickScoreCount();	break;
	case EGamblePhase::ShowResult:	TickShowResult();	break;
	case EGamblePhase::HoldResult:	TickHoldResult();	break;
	default: break;
	}
}

void UGambleWidget::SetPhase(EGamblePhase NewPhase)
{
	Phase = NewPhase;
	Elapsed = 0.f;
}

void UGambleWidget::TickInit()
{
	if (TargetGradeText)
		TargetGradeText->SetText(FText::FromString(FString::Printf(TEXT("목표 점수 : %d"),CachedData.TargetGrade)));
	
	UpdatePlayerScoreText(0);
	
	SetPhase(EGamblePhase::GemReveal);
}

void UGambleWidget::TickGemReveal()
{
	if (Elapsed < GemRevealInterval) return;
	
	if (CurrentGemIndex >= CachedData.bGemSuccessList.Num())
	{
		SetPhase(EGamblePhase::ScoreCount);
		return;
	}
	
	const bool bSuccess = CachedData.bGemSuccessList[CurrentGemIndex];
	
	if (bSuccess)
	{
		const int32 GemCount = CachedData.bGemSuccessList.Num();
		if (GemCount > 0)
		{
			DisplayedScore += CachedData.TotalGrade / GemCount;
			if (CurrentGemIndex == GemCount -1)
				DisplayedScore = CachedData.PlayerFinalGrade;
			UpdatePlayerScoreText(DisplayedScore);
		}
	}
	
	CurrentGemIndex++;
	
	Elapsed = 0.f;
}

void UGambleWidget::TickScoreCount()
{
	if (DisplayedScore == CachedData.PlayerFinalGrade)
	{
		SetPhase(EGamblePhase::ShowResult);
		return;
	}
	const float CountDuration = 0.3f;
	const float T = FMath::Clamp(Elapsed / CountDuration,0.f,1.f);
	const int32 Score = FMath::RoundToInt(FMath::Lerp((float)DisplayedScore,(float)CachedData.PlayerFinalGrade,T));
	UpdatePlayerScoreText(Score);
	
	if (T >= 1.f)
		SetPhase(EGamblePhase::ShowResult);
}

void UGambleWidget::TickShowResult()
{
	ShowResult(CachedData.bIsWin);
	SetPhase(EGamblePhase::HoldResult);
}

void UGambleWidget::TickHoldResult()
{
	if (Elapsed < ResultHoldDuration) return;
	
	SetPhase(EGamblePhase::Idle);
	OnGambleFinished.Broadcast(CachedData.bIsWin);
}

void UGambleWidget::UpdatePlayerScoreText(int32 Score)
{
	if (PlayerScoreText)
		PlayerScoreText->SetText(FText::FromString(FString::Printf(TEXT("내 점수: %d"), Score)));
}

void UGambleWidget::ShowResult(bool bIsWin)
{
	if (ResultText)
	{
		ResultText->SetText(FText::FromString(bIsWin ? TEXT("승리!") : TEXT("패배 ...")));
		ResultText->SetVisibility(ESlateVisibility::Visible);
	}
	if (ResultImage)
	{
		UTexture2D* Tex = bIsWin ? WinTexture : LoseTexture;
		if (Tex)
			ResultImage->SetBrushFromTexture(Tex);
		ResultImage->SetVisibility(ESlateVisibility::Visible);
	}
}

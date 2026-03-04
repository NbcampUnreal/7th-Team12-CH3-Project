#include "PrologueWidget.h"

#include "MediaPlayer.h"
#include "MediaSource.h"
#include "FileMediaSource.h"

void UPrologueWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(true);

	if (MediaPlayer)
	{
		MediaPlayer->OnEndReached.AddDynamic(this, &UPrologueWidget::HandleEndReached);
	}
}

FReply UPrologueWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	SkipPrologue();
	return FReply::Handled();
}

void UPrologueWidget::Start()
{
	if (!MediaPlayer || !MediaSource)
	{
		OnPrologueFinished.Broadcast();
		return;
	}

	if (!MediaPlayer->OpenSource(MediaSource))
	{
		OnPrologueFinished.Broadcast();
		return;
	}

	MediaPlayer->Play();
}

void UPrologueWidget::HandleEndReached()
{
	OnPrologueFinished.Broadcast();
}

void UPrologueWidget::SkipPrologue()
{
	if (bFinished) return;
	bFinished = true;

	if (MediaPlayer)
	{
		MediaPlayer->Close(); // 핵심: 영상/오디오 모두 종료
	}

	OnPrologueFinished.Broadcast();
}

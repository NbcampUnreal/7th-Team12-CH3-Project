#include "PrologueWidget.h"

#include "MediaPlayer.h"
#include "MediaSource.h"
#include "FileMediaSource.h"
#include "GameFramework/PlayerController.h"

void UPrologueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	SetVisibility(ESlateVisibility::Visible);

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

FReply UPrologueWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

	// 패키징 빌드에서 위젯이 키보드 입력을 받을 수 있도록
	// 입력 모드를 GameAndUI로 변경하고 이 위젯에 포커스를 설정
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
}

void UPrologueWidget::HandleEndReached()
{
	SkipPrologue();
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

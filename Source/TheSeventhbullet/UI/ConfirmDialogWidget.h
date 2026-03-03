////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * ConfirmDialogWidget - 범용 확인/취소 모달
 *
 * 텍스트와 확인/취소 버튼.
 * 매번 열 때 콜백이 자동 초기화되어 delegate 누적 없음.
 *
 * [사용법]
 *   UUIManager* UIMgr = UUIManager::Get(this);
 *   UConfirmDialogWidget* Dialog = Cast<UConfirmDialogWidget>(UIMgr->Open(UITags::ConfirmDialog));
 *   Dialog->ShowDialog(TEXT("정말 삭제하시겠습니까?"),
 *       FOnConfirmDialogResult::CreateLambda([](bool bConfirmed) { ... }));
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmDialogWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DELEGATE_OneParam(FOnConfirmDialogResult, bool /*bConfirmed*/);

UCLASS()
class THESEVENTHBULLET_API UConfirmDialogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowDialog(const FText& Message, FOnConfirmDialogResult InCallback);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;

private:
	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnCancelClicked();

	void CloseAndRespond(bool bConfirmed);

	FOnConfirmDialogResult ResultCallback;
};

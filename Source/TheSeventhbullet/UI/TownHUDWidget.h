////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * TownHUDWidget - 마을 Phase 상태 표시 HUD
 *
 * UIManager::Open(UITags::TownHUD)로 표시.
 * 현재 TownPhase에 따라 화면 상단에 안내 메시지를 표시.
 * RequestAccepted 상태에서는 의뢰 정보 + 이미지를 함께 표시.
 *
 * 블루프린트에서 WBP_TownHUD를 만들고 아래 위젯을 배치해야 함:
 *   - PhaseMessageText  (UTextBlock)
 *   - RequestImage      (UImage)
 *   - ShowAnimation     (UWidgetAnimation)
 *   - HideAnimation     (UWidgetAnimation)
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "System/TownPhase.h"
#include "TownHUDWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class THESEVENTHBULLET_API UTownHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PhaseMessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> RequestImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HideAnimation;

private:
	UFUNCTION()
	void OnTownPhaseChanged(ETownPhase NewPhase);

	UFUNCTION()
	void OnHideAnimFinished();

	void ApplyPhaseContent();
	void PlayShow();
	void PlayHide();

	FText BuildPhaseMessage() const;

	ETownPhase PendingPhase = ETownPhase::None;
};

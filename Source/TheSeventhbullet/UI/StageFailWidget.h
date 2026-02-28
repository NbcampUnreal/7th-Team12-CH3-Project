#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Wave/WaveStates/WaveState.h"
#include "StageFailWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UStageFailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetFailReason(EStageResult Result);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FailReasonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReturnButtonText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowAnimation;

private:
	UFUNCTION()
	void OnReturnClicked();

	EStageResult CachedResult = EStageResult::None;
};

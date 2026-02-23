#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgress(float Percent);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* SpinAnimation;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> LoadingBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LoadingText;
};

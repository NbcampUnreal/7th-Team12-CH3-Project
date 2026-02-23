#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionPromptWidget.generated.h"

class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPromptText(const FText& Text);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PromptText;
};

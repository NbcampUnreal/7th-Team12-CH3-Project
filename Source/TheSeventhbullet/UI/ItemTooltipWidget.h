#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.generated.h"

class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Tooltip")
	void SetItemInfo(const FText& Name, const FText& Desc);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescText;
};

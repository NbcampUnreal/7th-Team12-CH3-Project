#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RewardEntryWidget.generated.h"

struct FDroppedMaterialsData;
class UImage;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API URewardEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetData(const FDroppedMaterialsData& Data);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCountText;
};

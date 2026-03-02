#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageSuccessWidget.generated.h"

struct FDroppedMaterialsData;
class UButton;
class UTextBlock;
class UVerticalBox;
class URewardEntryWidget;

UCLASS()
class THESEVENTHBULLET_API UStageSuccessWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRewards(const TArray<FDroppedMaterialsData>& Rewards);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultTitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> RewardListBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URewardEntryWidget> RewardEntryClass;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowAnimation;

private:
	UFUNCTION()
	void OnReturnClicked();
};

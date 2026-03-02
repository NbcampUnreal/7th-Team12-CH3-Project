#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterInfoWidget.generated.h"

class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UCharacterInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Info")
	void RefreshInfo();

protected:
	virtual void NativeConstruct() override;

	// BindWidget — WBP에서 같은 이름의 TextBlock을 배치해야 함
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HPText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StaminaText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AttackText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DefenceText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CritChanceText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CritDamageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SpeedText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DayText;
};
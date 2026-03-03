#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GambleBettingDialogueWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBettingChose, int32, BettingGold);

UCLASS()
class THESEVENTHBULLET_API UGambleBettingDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Betting")
	void SetDialogue(const FText& InText);
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Betting")
	FOnBettingChose OnBettingChose;
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EasyBettingButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NormalBettingButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HardBettingButton;
	
	UPROPERTY(EditDefaultsOnly, Category = "Betting")
	FText DefaultDialogueText = FText::FromString(TEXT("얼마를 걸어볼텐가? 결과는 2가지 뿐이야. 2배를 얻거나 혹은 잃거나."));
	
private:
	UFUNCTION()
	void OnEasyBettingClicked();
	UFUNCTION()
	void OnNormalBettingClicked();
	UFUNCTION()
	void OnHardBettingClicked();
};

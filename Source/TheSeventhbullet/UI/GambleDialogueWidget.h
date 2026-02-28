#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GambleDialogueWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UImage;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueConfirmed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueCancelled);

UCLASS()
class THESEVENTHBULLET_API UGambleDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetDialogue(const FText& InText);
	
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueConfirmed OnConfirmed;
	
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueCancelled OnCancelled;
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	FText DefaultDialogueText = FText::FromString(TEXT("허허, 운명에 도전해볼 텐가?\n소울젬을 걸고 나와 한판 해보시게"));
	
private:
	UFUNCTION()
	void OnConfirmClicked();
	
	UFUNCTION()
	void OnCancelClicked();
};

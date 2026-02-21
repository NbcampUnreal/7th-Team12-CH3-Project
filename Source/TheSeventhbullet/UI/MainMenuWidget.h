#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NewGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(EditDefaultsOnly, Category = "MainMenu")
	FName GameMapName = TEXT("GameLevel");

	UPROPERTY(EditDefaultsOnly, Category = "MainMenu")
	FSlateColor NormalTextColor = FSlateColor(FLinearColor::White);

	UPROPERTY(EditDefaultsOnly, Category = "MainMenu")
	FSlateColor HoveredTextColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));

private:
	void SetupButtonHover(UButton* Button);
	UTextBlock* FindChildTextBlock(UWidget* Parent);

	UFUNCTION()
	void OnAnyButtonHovered();

	UFUNCTION()
	void OnAnyButtonUnhovered();

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnQuitClicked();

	TMap<UButton*, UTextBlock*> ButtonTextMap;
};
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EscMenuWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UEscMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackToMenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	UPROPERTY(EditDefaultsOnly, Category = "EscMenu")
	FName MainMenuMapName = TEXT("MainMenuLevel");

	UPROPERTY(EditDefaultsOnly, Category = "EscMenu")
	FSlateColor NormalTextColor = FSlateColor(FLinearColor::White);

	UPROPERTY(EditDefaultsOnly, Category = "EscMenu")
	FSlateColor HoveredTextColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));

private:
	void SetupButtonHover(UButton* Button);
	UTextBlock* FindChildTextBlock(UWidget* Parent);

	UFUNCTION()
	void OnAnyButtonHovered();
	UFUNCTION()
	void OnAnyButtonUnhovered();

	UFUNCTION()
	void OnResumeClicked();
	UFUNCTION()
	void OnSettingsClicked();
	UFUNCTION()
	void OnBackToMenuClicked();
	UFUNCTION()
	void OnQuitGameClicked();

	TMap<UButton*, UTextBlock*> ButtonTextMap;
};
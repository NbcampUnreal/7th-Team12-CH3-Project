#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrologueWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrologueFinished);

UCLASS()
class THESEVENTHBULLET_API UPrologueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPrologueFinished OnPrologueFinished;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Prologue")
	TObjectPtr<class UMediaPlayer> MediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Prologue")
	TObjectPtr<class UMediaSource> MediaSource;

	UFUNCTION(BlueprintCallable)
	void Start();

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UFUNCTION()
	void HandleEndReached();
	UFUNCTION()
	void SkipPrologue();
private:
	bool bFinished = false;
};

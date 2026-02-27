#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/TableRowTypes.h"
#include "RequestSlotWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DELEGATE_OneParam(FOnRequestAccepted, int32);

UCLASS()
class THESEVENTHBULLET_API URequestSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRequestInfo(int32 InRequestID, const FRequestRowData& Data);

	FOnRequestAccepted OnRequestAccepted;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RequestNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WaveCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AcceptButton;

private:
	UFUNCTION()
	void OnAcceptClicked();

	int32 RequestID = INDEX_NONE;
};

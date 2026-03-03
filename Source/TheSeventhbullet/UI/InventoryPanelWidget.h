#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanelWidget.generated.h"

class UInventoryWidget;
class UCharacterInfoWidget;

UCLASS()
class THESEVENTHBULLET_API UInventoryPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterInfoWidget> CharacterInfoWidget;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * CraftResultWidget - 소울젬 제작 결과 Modal 팝업
 *
 * 제작 성공 시 UIManager의 Modal 레이어로 표시.
 * 소울젬의 이름, 등급, 스탯, 특수옵션을 보여주고 확인 버튼으로 닫음.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoulGem/SoulGemInstance.h"
#include "CraftResultWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;

UCLASS()
class THESEVENTHBULLET_API UCraftResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Crafting")
	void SetResult(const FSoulGemInstance& SoulGem, FPrimaryAssetId GemAssetID);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ResultIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultInfoText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;

private:
	UFUNCTION()
	void OnConfirmClicked();

	FString BuildResultInfoString(const FSoulGemInstance& SoulGem) const;
	void LoadIconFromDA(class UItemDataAsset* GemData);
	void SetIcon(UTexture2D* Texture);
};

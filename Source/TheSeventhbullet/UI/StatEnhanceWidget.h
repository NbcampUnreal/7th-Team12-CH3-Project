#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/StatusTypes.h"
#include "StatEnhanceWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UStatEnhanceWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "StatEnhancer")
	void InitWidget(UStatEnhancerComponent* InComponent);

	UFUNCTION(BlueprintCallable, Category = "StatEnhancer")
	void RefreshAll();
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;
	
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> HP_NameText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> HP_LevelText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> HP_BonusText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> HP_CostText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton>    HP_UpgradeBtn;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> ATK_NameText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> ATK_LevelText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> ATK_BonusText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> ATK_CostText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton>    ATK_UpgradeBtn;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> DEF_NameText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> DEF_LevelText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> DEF_BonusText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> DEF_CostText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton>    DEF_UpgradeBtn;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> Stamina_NameText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> Stamina_LevelText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> Stamina_BonusText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> Stamina_CostText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton>    Stamina_UpgradeBtn;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> CloseButton;
private:
	UPROPERTY()
	TObjectPtr<UStatEnhancerComponent> CachedComponent;
	
	UFUNCTION() void OnClick_HP();
	UFUNCTION() void OnClick_ATK();
	UFUNCTION() void OnClick_DEF();
	UFUNCTION() void OnClick_Stamina();
	UFUNCTION() void OnClick_Close();
	
	void RefreshStatRow(
	EStatusType StatType,
	UTextBlock* LevelText,
	UTextBlock* BonusText,
	UTextBlock* CostText,
	UButton*    UpgradeBtn);

	void TryUpgrade(EStatusType StatType);

	FString GetBonusString(EStatusType StatType, int32 Level) const;
};

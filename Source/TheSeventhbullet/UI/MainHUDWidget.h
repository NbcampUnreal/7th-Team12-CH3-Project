////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * MainHUDWidget - 인게임 HUD (HP Bar + 탄약 정보)
 *
 * UIManager::ShowByTag(UITags::HUD)로 표시.
 * Crosshair는 별도 CrosshairWidget으로 분리.
 * 블루프린트에서 WBP_MainHUD를 만들고 BindWidget 위젯들을 배치해야 함.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void UpdateHP(float CurrentHP, float MaxHP);
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;
};
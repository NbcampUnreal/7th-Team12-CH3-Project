////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * MainHUDWidget - 인게임 HUD (HP Bar + 탄약 정보)
 *
 * UIManager::Open(UITags::HUD)로 표시.
 * Crosshair는 별도 CrosshairWidget으로 분리.
 * 블루프린트에서 WBP_MainHUD를 만들고 BindWidget 위젯들을 배치해야 함.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/TableRowTypes.h"
#include "MainHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UOverlay;
class UItemDropNotifyWidget;

UCLASS()
class THESEVENTHBULLET_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void UpdateHP(float CurrentHP, float MaxHP);
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

	// --- Wave Info ---
	void ShowWaveInfo(int32 WaveNumber, float Countdown);
	void UpdateWaveTimer(float RemainingTime);
	void UpdateMonsterCount(int32 AliveCount);
	void ShowWaveClear();
	void ShowIntermission(float RemainingTime);
	void HideWaveInfo();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// --- HUD ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

	// --- Wave Info ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WaveInfoText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WaveTimerText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MonsterCountText;

	// --- 드랍 알림 ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> DropNotifyBox;

	UPROPERTY(EditDefaultsOnly, Category = "UI|DropNotify")
	TSubclassOf<UItemDropNotifyWidget> NotifyEntryClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI|DropNotify")
	float NotifyDuration = 1.0f;

private:
	UFUNCTION()
	void OnItemDropped(const TArray<FDroppedMaterialsData>& DroppedMaterials);

	UFUNCTION()
	void OnHPChangedHandler(float CurrentHP, float MaxHP);
	UFUNCTION()
	void OnAmmoChangedHandler(int32 CurrentAmmo, int32 MaxAmmo);

	void ShowNextNotify();
	void OnCurrentNotifyHideFinished();

	// 대기 큐
	TArray<FDroppedMaterialsData> PendingNotifies;

	// 현재 표시 중인 알림
	UPROPERTY()
	TObjectPtr<UItemDropNotifyWidget> CurrentNotify;

	FTimerHandle NotifyTimerHandle;

	// --- Bar Lerp ---
	float HPBarTarget = 1.f;
	float HPBarCurrent = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "UI|HUD")
	float BarLerpSpeed = 8.f;
};

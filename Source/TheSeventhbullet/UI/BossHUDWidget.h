////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * BossHUDWidget - 보스 전투 전용 HUD
 *
 * MainHUDWidget과 동일한 HP/탄약/포션/스킬 기능을 독립적으로 구현.
 * WaveInfo, Drop 알림 없음.
 * BossHPBar를 추가하여 보스 체력 표시.
 *
 * 블루프린트에서 WBP_BossHUD를 만들고 BindWidget 위젯들을 배치해야 함.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UBossEnemyActorComponent;

UCLASS()
class THESEVENTHBULLET_API UBossHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|BossHUD")
	void UpdateHP(float CurrentHP, float MaxHP);
	UFUNCTION(BlueprintCallable, Category = "UI|BossHUD")
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION(BlueprintCallable, Category = "UI|BossHUD")
	void UpdatePotionCount(int32 Count);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// --- 플레이어 HUD ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PotionCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PotionCooldownBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SkillCooldownBar;

	// --- 보스 HP ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> BossHPBar;

private:
	// 캐릭터 델리게이트 핸들러
	UFUNCTION()
	void OnHPChangedHandler(float CurrentHP, float MaxHP);
	UFUNCTION()
	void OnAmmoChangedHandler(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION()
	void OnPotionChangedHandler(int32 Count);
	UFUNCTION()
	void OnPotionCooldownStartedHandler(float CoolTime);
	UFUNCTION()
	void OnSkillCooldownStartedHandler(float CoolTime);

	// 보스 델리게이트 핸들러
	UFUNCTION()
	void OnBossHealthChanged(float CurrentHP, float MaxHP);
	void FindAndBindBoss();

	UPROPERTY()
	TObjectPtr<UBossEnemyActorComponent> CachedBossComp;

	// --- HP Bar Lerp ---
	float HPBarTarget = 1.f;
	float HPBarCurrent = 1.f;

	// --- Boss HP Bar Lerp ---
	float BossHPBarTarget = 1.f;
	float BossHPBarCurrent = 1.f;

	// --- Potion Cooldown ---
	float CooldownTotal = 0.f;
	float CooldownRemaining = 0.f;
	bool bCooldownActive = false;

	// --- Skill Cooldown ---
	float SkillCooldownTotal = 0.f;
	float SkillCooldownRemaining = 0.f;
	bool bSkillCooldownActive = false;

	UPROPERTY(EditDefaultsOnly, Category = "UI|BossHUD")
	float BarLerpSpeed = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI|BossHUD")
	float BossBarLerpSpeed = 6.f;
};

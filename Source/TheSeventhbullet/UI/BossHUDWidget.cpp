#include "BossHUDWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/MainCharacter.h"
#include "Character/Component/CombatComponent.h"
#include "Enemy/Boss/BossEnemyActorComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

void UBossHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateHP(1.f, 1.f);
	UpdateAmmo(0, 0);

	// 캐릭터 델리게이트 바인딩
	if (AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Character->OnHPChanged.AddDynamic(this, &UBossHUDWidget::OnHPChangedHandler);
		UpdateHP(Character->GetCurrentHP(), Character->GetMaxHP());

		Character->OnPotionChanged.AddDynamic(this, &UBossHUDWidget::OnPotionChangedHandler);
		Character->OnPotionCooldownStarted.AddDynamic(this, &UBossHUDWidget::OnPotionCooldownStartedHandler);

		if (Character->InventoryComponent)
		{
			FPrimaryAssetId PotionID(FPrimaryAssetType("Item"), FName("DA_HealthPotion"));
			UpdatePotionCount(Character->InventoryComponent->GetCountByID(PotionID));
		}
		if (PotionCooldownBar)
		{
			PotionCooldownBar->SetPercent(0.f);
		}

		Character->OnSkillCooldownStarted.AddDynamic(this, &UBossHUDWidget::OnSkillCooldownStartedHandler);
		if (SkillCooldownBar)
		{
			SkillCooldownBar->SetPercent(0.f);
		}

		if (UCombatComponent* Combat = Character->CombatComponent)
		{
			Combat->OnAmmoChanged.AddDynamic(this, &UBossHUDWidget::OnAmmoChangedHandler);
			UpdateAmmo(Combat->GetCurrentAmmo(), Combat->GetMaxAmmo());
		}
	}

	// BossHPBar 초기화
	BossHPBarTarget = 1.f;
	BossHPBarCurrent = 1.f;
	if (BossHPBar)
	{
		BossHPBar->SetPercent(1.f);
	}

	FindAndBindBoss();
}

void UBossHUDWidget::NativeDestruct()
{
	if (CachedBossComp)
	{
		CachedBossComp->OnBossHealthChanged.RemoveDynamic(this, &UBossHUDWidget::OnBossHealthChanged);
		CachedBossComp = nullptr;
	}

	if (AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Character->OnHPChanged.RemoveDynamic(this, &UBossHUDWidget::OnHPChangedHandler);
		Character->OnPotionChanged.RemoveDynamic(this, &UBossHUDWidget::OnPotionChangedHandler);
		Character->OnPotionCooldownStarted.RemoveDynamic(this, &UBossHUDWidget::OnPotionCooldownStartedHandler);
		Character->OnSkillCooldownStarted.RemoveDynamic(this, &UBossHUDWidget::OnSkillCooldownStartedHandler);

		if (UCombatComponent* Combat = Character->CombatComponent)
		{
			Combat->OnAmmoChanged.RemoveDynamic(this, &UBossHUDWidget::OnAmmoChangedHandler);
		}
	}

	Super::NativeDestruct();
}

void UBossHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// HP Bar Lerp
	if (!FMath::IsNearlyEqual(HPBarCurrent, HPBarTarget, 0.001f))
	{
		HPBarCurrent = FMath::FInterpTo(HPBarCurrent, HPBarTarget, InDeltaTime, BarLerpSpeed);
		if (HPBar)
		{
			HPBar->SetPercent(HPBarCurrent);
		}
	}

	// 보스 바인딩 재시도 (NativeConstruct 시점에 보스가 아직 없을 수 있음)
	if (!CachedBossComp)
	{
		FindAndBindBoss();
	}

	// Boss HP Bar Lerp
	if (!FMath::IsNearlyEqual(BossHPBarCurrent, BossHPBarTarget, 0.001f))
	{
		BossHPBarCurrent = FMath::FInterpTo(BossHPBarCurrent, BossHPBarTarget, InDeltaTime, BossBarLerpSpeed);
		if (BossHPBar)
		{
			BossHPBar->SetPercent(BossHPBarCurrent);
		}
	}

	// Potion Cooldown Bar
	if (bCooldownActive)
	{
		CooldownRemaining -= InDeltaTime;
		float Percent = FMath::Clamp(CooldownRemaining / CooldownTotal, 0.f, 1.f);
		if (PotionCooldownBar)
		{
			PotionCooldownBar->SetPercent(Percent);
		}
		if (CooldownRemaining <= 0.f)
		{
			bCooldownActive = false;
			if (PotionCooldownBar)
			{
				PotionCooldownBar->SetPercent(0.f);
			}
		}
	}

	// Skill Cooldown Bar
	if (bSkillCooldownActive)
	{
		SkillCooldownRemaining -= InDeltaTime;
		float Percent = FMath::Clamp(SkillCooldownRemaining / SkillCooldownTotal, 0.f, 1.f);
		if (SkillCooldownBar)
		{
			SkillCooldownBar->SetPercent(Percent);
		}
		if (SkillCooldownRemaining <= 0.f)
		{
			bSkillCooldownActive = false;
			if (SkillCooldownBar)
			{
				SkillCooldownBar->SetPercent(0.f);
			}
		}
	}
}

void UBossHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	HPBarTarget = (MaxHP > 0.f) ? FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f) : 0.f;

	if (FMath::IsNearlyEqual(HPBarCurrent, HPBarTarget, 0.001f) && HPBar)
	{
		HPBarCurrent = HPBarTarget;
		HPBar->SetPercent(HPBarCurrent);
	}
}

void UBossHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
	}
}

void UBossHUDWidget::UpdatePotionCount(int32 Count)
{
	if (PotionCountText)
	{
		PotionCountText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), Count)));
	}
}

void UBossHUDWidget::OnHPChangedHandler(float CurrentHP, float MaxHP)
{
	UpdateHP(CurrentHP, MaxHP);
}

void UBossHUDWidget::OnAmmoChangedHandler(int32 CurrentAmmo, int32 MaxAmmo)
{
	UpdateAmmo(CurrentAmmo, MaxAmmo);
}

void UBossHUDWidget::OnPotionChangedHandler(int32 Count)
{
	UpdatePotionCount(Count);
}

void UBossHUDWidget::OnPotionCooldownStartedHandler(float CoolTime)
{
	CooldownTotal = CoolTime;
	CooldownRemaining = CoolTime;
	bCooldownActive = true;
	if (PotionCooldownBar)
	{
		PotionCooldownBar->SetPercent(1.f);
	}
}

void UBossHUDWidget::OnSkillCooldownStartedHandler(float CoolTime)
{
	SkillCooldownTotal = CoolTime;
	SkillCooldownRemaining = CoolTime;
	bSkillCooldownActive = true;
	if (SkillCooldownBar)
	{
		SkillCooldownBar->SetPercent(1.f);
	}
}

void UBossHUDWidget::OnBossHealthChanged(float CurrentHP, float MaxHP)
{
	BossHPBarTarget = (MaxHP > 0.f) ? FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f) : 0.f;
}

void UBossHUDWidget::FindAndBindBoss()
{
	TArray<AActor*> BossActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Boss"), BossActors);

	for (AActor* Actor : BossActors)
	{
		UBossEnemyActorComponent* BossComp = Actor->FindComponentByClass<UBossEnemyActorComponent>();
		if (BossComp)
		{
			CachedBossComp = BossComp;
			BossComp->OnBossHealthChanged.AddDynamic(this, &UBossHUDWidget::OnBossHealthChanged);
			break;
		}
	}
}

#include "MainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "ItemDropNotifyWidget.h"
#include "System/MainGameMode.h"
#include "Character/MainCharacter.h"
#include "Character/Component/CombatComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

void UMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateHP(1.f, 1.f);
	UpdateAmmo(0, 0);

	if (DropNotifyBox)
	{
		DropNotifyBox->ClearChildren();
	}

	if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnMaterialDroppedMonsterKilled.AddDynamic(this, &UMainHUDWidget::OnItemDropped);
	}

	// 캐릭터 델리게이트 바인딩
	if (AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Character->OnHPChanged.AddDynamic(this, &UMainHUDWidget::OnHPChangedHandler);

		// 초기값 반영
		UpdateHP(Character->GetCurrentHP(), Character->GetMaxHP());

		Character->OnPotionChanged.AddDynamic(this, &UMainHUDWidget::OnPotionChangedHandler);
		Character->OnPotionCooldownStarted.AddDynamic(this, &UMainHUDWidget::OnPotionCooldownStartedHandler);

		// 초기 물약 수량 표시
		if (Character->InventoryComponent)
		{
			FPrimaryAssetId PotionID(FPrimaryAssetType("Item"), FName("DA_HealthPotion"));
			UpdatePotionCount(Character->InventoryComponent->GetCountByID(PotionID));
		}
		if (PotionCooldownBar)
		{
			PotionCooldownBar->SetPercent(0.f);
		}

		Character->OnSkillCooldownStarted.AddDynamic(this, &UMainHUDWidget::OnSkillCooldownStartedHandler);
		if (SkillCooldownBar)
		{
			SkillCooldownBar->SetPercent(0.f);
		}

		if (UCombatComponent* Combat = Character->CombatComponent)
		{
			Combat->OnAmmoChanged.AddDynamic(this, &UMainHUDWidget::OnAmmoChangedHandler);
			UpdateAmmo(Combat->GetCurrentAmmo(), Combat->GetMaxAmmo());
		}
	}
}

void UMainHUDWidget::NativeDestruct()
{
	if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnMaterialDroppedMonsterKilled.RemoveDynamic(this, &UMainHUDWidget::OnItemDropped);
	}

	if (AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Character->OnHPChanged.RemoveDynamic(this, &UMainHUDWidget::OnHPChangedHandler);
		Character->OnPotionChanged.RemoveDynamic(this, &UMainHUDWidget::OnPotionChangedHandler);
		Character->OnPotionCooldownStarted.RemoveDynamic(this, &UMainHUDWidget::OnPotionCooldownStartedHandler);
		Character->OnSkillCooldownStarted.RemoveDynamic(this, &UMainHUDWidget::OnSkillCooldownStartedHandler);

		if (UCombatComponent* Combat = Character->CombatComponent)
		{
			Combat->OnAmmoChanged.RemoveDynamic(this, &UMainHUDWidget::OnAmmoChangedHandler);
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(NotifyTimerHandle);
	}

	CurrentNotify = nullptr;
	PendingNotifies.Empty();

	Super::NativeDestruct();
}

void UMainHUDWidget::OnItemDropped(const TArray<FDroppedMaterialsData>& DroppedMaterials)
{
	for (const FDroppedMaterialsData& DropData : DroppedMaterials)
	{
		PendingNotifies.Add(DropData);
	}

	// 현재 표시 중인 알림이 없으면 바로 표시
	if (!CurrentNotify)
	{
		ShowNextNotify();
	}
}

void UMainHUDWidget::ShowNextNotify()
{
	if (PendingNotifies.Num() == 0 || !NotifyEntryClass || !DropNotifyBox)
	{
		return;
	}

	// 큐에서 꺼내기
	FDroppedMaterialsData DropData = PendingNotifies[0];
	PendingNotifies.RemoveAt(0);

	UItemDropNotifyWidget* Entry = CreateWidget<UItemDropNotifyWidget>(this, NotifyEntryClass);
	if (!Entry)
	{
		return;
	}

	Entry->SetDropInfo(DropData);
	DropNotifyBox->ClearChildren();
	DropNotifyBox->AddChild(Entry);
	CurrentNotify = Entry;

	// 타이머 후 사라지는 애니메이션 시작
	GetWorld()->GetTimerManager().SetTimer(
		NotifyTimerHandle,
		[this]()
		{
			if (CurrentNotify)
			{
				CurrentNotify->OnHideFinished.BindUObject(this, &UMainHUDWidget::OnCurrentNotifyHideFinished);
				CurrentNotify->PlayHide();
			}
		},
		NotifyDuration,
		false
	);
}

void UMainHUDWidget::OnCurrentNotifyHideFinished()
{
	if (CurrentNotify)
	{
		CurrentNotify->RemoveFromParent();
		CurrentNotify = nullptr;
	}

	// 큐에 다음 알림이 있으면 표시
	ShowNextNotify();
}

// --- Wave Info ---
void UMainHUDWidget::ShowWaveInfo(int32 WaveNumber, float Countdown)
{
	if (WaveInfoText)
	{
		WaveInfoText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), WaveNumber)));
		WaveInfoText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (WaveTimerText)
	{
		WaveTimerText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), FMath::CeilToFloat(Countdown))));
		WaveTimerText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (MonsterCountText)
	{
		MonsterCountText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainHUDWidget::UpdateWaveTimer(float RemainingTime)
{
	if (WaveTimerText)
	{
		int32 Minutes = FMath::FloorToInt(RemainingTime / 60.f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(RemainingTime, 60.f));
		WaveTimerText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
	}
}

void UMainHUDWidget::UpdateMonsterCount(int32 AliveCount)
{
	if (MonsterCountText)
	{
		MonsterCountText->SetText(FText::FromString(FString::Printf(TEXT("Remaining : %d"), AliveCount)));
		MonsterCountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UMainHUDWidget::ShowWaveClear()
{
	if (WaveInfoText)
	{
		WaveInfoText->SetText(FText::FromString(TEXT("Wave Clear!")));
		WaveInfoText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (WaveTimerText)
	{
		WaveTimerText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (MonsterCountText)
	{
		MonsterCountText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainHUDWidget::ShowIntermission(float RemainingTime)
{
	if (WaveInfoText)
	{
		WaveInfoText->SetText(FText::FromString(TEXT("Intermission")));
		WaveInfoText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (WaveTimerText)
	{
		WaveTimerText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), FMath::CeilToFloat(RemainingTime))));
		WaveTimerText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (MonsterCountText)
	{
		MonsterCountText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainHUDWidget::HideWaveInfo()
{
	if (WaveInfoText) WaveInfoText->SetVisibility(ESlateVisibility::Collapsed);
	if (WaveTimerText) WaveTimerText->SetVisibility(ESlateVisibility::Collapsed);
	if (MonsterCountText) MonsterCountText->SetVisibility(ESlateVisibility::Collapsed);
}

void UMainHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	HPBarTarget = (MaxHP > 0.f) ? FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f) : 0.f;

	// 초기 상태에서 Lerp 없이 바로 반영
	if (FMath::IsNearlyEqual(HPBarCurrent, HPBarTarget, 0.001f) && HPBar)
	{
		HPBarCurrent = HPBarTarget;
		HPBar->SetPercent(HPBarCurrent);
	}
}

void UMainHUDWidget::OnHPChangedHandler(float CurrentHP, float MaxHP)
{
	UpdateHP(CurrentHP, MaxHP);
}

void UMainHUDWidget::OnAmmoChangedHandler(int32 CurrentAmmo, int32 MaxAmmo)
{
	UpdateAmmo(CurrentAmmo, MaxAmmo);
}

void UMainHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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

void UMainHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
	}
}

void UMainHUDWidget::UpdatePotionCount(int32 Count)
{
	if (PotionCountText)
	{
		PotionCountText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), Count)));
	}
}

void UMainHUDWidget::OnPotionChangedHandler(int32 Count)
{
	UpdatePotionCount(Count);
}

void UMainHUDWidget::OnPotionCooldownStartedHandler(float CoolTime)
{
	CooldownTotal = CoolTime;
	CooldownRemaining = CoolTime;
	bCooldownActive = true;
	if (PotionCooldownBar)
	{
		PotionCooldownBar->SetPercent(1.f);
	}
}

void UMainHUDWidget::OnSkillCooldownStartedHandler(float CoolTime)
{
	SkillCooldownTotal = CoolTime;
	SkillCooldownRemaining = CoolTime;
	bSkillCooldownActive = true;
	if (SkillCooldownBar)
	{
		SkillCooldownBar->SetPercent(1.f);
	}
}

#include "MainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "ItemDropNotifyWidget.h"
#include "System/MainGameMode.h"

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
}

void UMainHUDWidget::NativeDestruct()
{
	if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnMaterialDroppedMonsterKilled.RemoveDynamic(this, &UMainHUDWidget::OnItemDropped);
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

//스탯관리하는 부분에서 BroadCast받아 올 예정
void UMainHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	if (HPBar)
	{
		float Ratio = (MaxHP > 0.f) ? FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f) : 0.f;
		HPBar->SetPercent(Ratio);
	}
}

//스탯관리하는 부분에서 BroadCast받아 올 예정
void UMainHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
	}
}

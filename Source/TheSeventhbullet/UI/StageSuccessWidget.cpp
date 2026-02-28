#include "StageSuccessWidget.h"
#include "RewardEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "System/MainGameMode.h"
#include "Data/TableRowTypes.h"

void UStageSuccessWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResultTitleText)
	{
		ResultTitleText->SetText(FText::FromString(TEXT("Mission Clear!")));
	}

	if (ReturnButton && !ReturnButton->OnClicked.IsAlreadyBound(this, &UStageSuccessWidget::OnReturnClicked))
	{
		ReturnButton->OnClicked.AddDynamic(this, &UStageSuccessWidget::OnReturnClicked);
	}

	if (ShowAnimation)
	{
		PlayAnimation(ShowAnimation);
	}
}

void UStageSuccessWidget::SetRewards(const TArray<FDroppedMaterialsData>& Rewards)
{
	UE_LOG(LogTemp, Warning, TEXT("[StageSuccessWidget] SetRewards called - Num=%d"), Rewards.Num());
	UE_LOG(LogTemp, Warning, TEXT("[StageSuccessWidget] RewardListBox=%s, RewardEntryClass=%s"),
		RewardListBox ? TEXT("OK") : TEXT("NULL"),
		RewardEntryClass ? TEXT("OK") : TEXT("NULL"));

	if (!RewardListBox || !RewardEntryClass) return;

	RewardListBox->ClearChildren();

	for (int32 i = 0; i < Rewards.Num(); i++)
	{
		const FDroppedMaterialsData& Reward = Rewards[i];
		UE_LOG(LogTemp, Warning, TEXT("[StageSuccessWidget] Creating Entry [%d] Material=%s, Count=%d"),
			i, *Reward.Material.GetAssetName(), Reward.Count);

		URewardEntryWidget* Entry = CreateWidget<URewardEntryWidget>(this, RewardEntryClass);
		if (Entry)
		{
			Entry->SetData(Reward);
			RewardListBox->AddChildToVerticalBox(Entry);
			UE_LOG(LogTemp, Warning, TEXT("[StageSuccessWidget] Entry [%d] added to ListBox"), i);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[StageSuccessWidget] CreateWidget FAILED for Entry [%d]"), i);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[StageSuccessWidget] Final children count=%d"), RewardListBox->GetChildrenCount());
}

void UStageSuccessWidget::OnReturnClicked()
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->ReturnToTown();
	}
}
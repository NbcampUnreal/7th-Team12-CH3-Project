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
	if (!RewardListBox || !RewardEntryClass) return;

	RewardListBox->ClearChildren();

	for (const FDroppedMaterialsData& Reward : Rewards)
	{
		URewardEntryWidget* Entry = CreateWidget<URewardEntryWidget>(this, RewardEntryClass);
		if (Entry)
		{
			Entry->SetData(Reward);
			RewardListBox->AddChildToVerticalBox(Entry);
		}
	}
}

void UStageSuccessWidget::OnReturnClicked()
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		GM->ReturnToTown();
	}
}
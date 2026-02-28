#include "StageSuccessWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Manager/UIManager.h"
#include "UITags.h"
#include "System/MainGameMode.h"
#include "Data/TableRowTypes.h"

void UStageSuccessWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResultTitleText)
	{
		ResultTitleText->SetText(FText::FromString(TEXT("스테이지 클리어!")));
	}

	if (ReturnButton && !ReturnButton->OnClicked.IsAlreadyBound(this, &UStageSuccessWidget::OnReturnClicked))
	{
		ReturnButton->OnClicked.AddDynamic(this, &UStageSuccessWidget::OnReturnClicked);
	}
}

void UStageSuccessWidget::SetRewards(const TArray<FDroppedMaterialsData>& Rewards)
{
	UE_LOG(LogTemp, Log, TEXT("StageSuccessWidget: Received %d reward entries"), Rewards.Num());
	for (const FDroppedMaterialsData& Reward : Rewards)
	{
		UE_LOG(LogTemp, Log, TEXT("  - Material: %s x%d"),
			*Reward.Material.GetAssetName(), Reward.Count);
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

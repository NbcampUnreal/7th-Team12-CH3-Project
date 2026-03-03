#include "StageSuccessWidget.h"
#include "RewardEntryWidget.h"
#include "Character/MainCharacter.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "System/MainGameMode.h"
#include "Data/TableRowTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/SyncDataManager.h"

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

	// 2회차 이후 캐시 재사용 시에도 애니메이션 재생
	if (ShowAnimation)
	{
		PlayAnimation(ShowAnimation);
	}
}

void UStageSuccessWidget::OnReturnClicked()
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM)
	{
		USyncDataManager* DataManager = USyncDataManager::Get(this);
		FRequestRowData Data = DataManager->GetRequestData(GM->GetCurrentRequestID());
		AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
		Character->AddGold(Data.RequestGold);
		GM->ReturnToTown();
	}
}

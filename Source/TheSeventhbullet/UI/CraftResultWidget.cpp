#include "CraftResultWidget.h"
#include "UITags.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Manager/UIManager.h"
#include "Manager/AsyncDataManager.h"
#include "DataAsset/ItemDataAsset.h"
#include "Data/StatusTypes.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UCraftResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton && !ConfirmButton->OnClicked.IsAlreadyBound(this, &UCraftResultWidget::OnConfirmClicked))
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UCraftResultWidget::OnConfirmClicked);
	}
}

void UCraftResultWidget::SetResult(const FSoulGemInstance& SoulGem, FPrimaryAssetId GemAssetID)
{
	if (ResultNameText)
	{
		ResultNameText->SetText(SoulGem.GemName);
	}

	if (ResultInfoText)
	{
		ResultInfoText->SetText(FText::FromString(BuildResultInfoString(SoulGem)));
	}

	// DA에서 아이콘 로드
	if (ResultIcon)
	{
		UAsyncDataManager* DataMgr = UAsyncDataManager::Get(this);
		if (DataMgr)
		{
			UItemDataAsset* GemData = Cast<UItemDataAsset>(DataMgr->GetLoadedAsset(GemAssetID));
			if (GemData)
			{
				UTexture2D* IconTexture = GemData->Icon.Get();
				if (IconTexture)
				{
					SetIcon(IconTexture);
				}
				else if (!GemData->Icon.IsNull())
				{
					TSoftObjectPtr<UTexture2D> IconPtr = GemData->Icon;
					FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
					Streamable.RequestAsyncLoad(
						IconPtr.ToSoftObjectPath(),
						FStreamableDelegate::CreateWeakLambda(this, [this, IconPtr]()
						{
							SetIcon(IconPtr.Get());
						})
					);
				}
			}
		}
	}
}

void UCraftResultWidget::OnConfirmClicked()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::CraftResult);
	}
}

FString UCraftResultWidget::BuildResultInfoString(const FSoulGemInstance& SoulGem) const
{
	FString Info;
	Info += FString::Printf(TEXT("등급: %d\n"), SoulGem.Grade);

	for (const FStatusModifier& Mod : SoulGem.StatusModifiers)
	{
		FString StatusName;
		switch (Mod.Status)
		{
		case EStatusType::HP:               StatusName = TEXT("HP"); break;
		case EStatusType::Stamina:          StatusName = TEXT("Stamina"); break;
		case EStatusType::ATK:              StatusName = TEXT("ATK"); break;
		case EStatusType::DEF:              StatusName = TEXT("DEF"); break;
		case EStatusType::DodgeCost:        StatusName = TEXT("DodgeCost"); break;
		case EStatusType::MaxSpeed:         StatusName = TEXT("MaxSpeed"); break;
		case EStatusType::SprintMultiplier: StatusName = TEXT("SprintMultiplier"); break;
		case EStatusType::CritChance:       StatusName = TEXT("CritChance"); break;
		case EStatusType::CritDamage:       StatusName = TEXT("CritDamage"); break;
		default:                            StatusName = TEXT("Unknown"); break;
		}

		if (Mod.CalculationMethod == EStatusCalculationMethod::AddFlat)
		{
			Info += FString::Printf(TEXT("%s +%.0f\n"), *StatusName, Mod.Value);
		}
		else
		{
			Info += FString::Printf(TEXT("%s x%.1f\n"), *StatusName, Mod.Value);
		}
	}

	for (ESpecialOptions Opt : SoulGem.SpecialOptions)
	{
		switch (Opt)
		{
		case ESpecialOptions::ReloadSpeed:
			Info += TEXT("특수: 재장전 속도 증가\n");
			break;
		case ESpecialOptions::DoubleShot:
			Info += TEXT("특수: 이중 발사\n");
			break;
		default:
			break;
		}
	}

	return Info;
}

void UCraftResultWidget::SetIcon(UTexture2D* Texture)
{
	if (!ResultIcon) return;

	if (Texture)
	{
		ResultIcon->SetBrushFromTexture(Texture);
		ResultIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ResultIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

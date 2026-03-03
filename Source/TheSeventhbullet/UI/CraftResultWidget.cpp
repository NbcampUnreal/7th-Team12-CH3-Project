#include "CraftResultWidget.h"
#include "UITags.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Manager/UIManager.h"
#include "Manager/AsyncDataManager.h"
#include "DataAsset/ItemDataAsset.h"
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
	UE_LOG(LogTemp, Warning, TEXT("[CraftResult] SetResult 아이콘 로드 시작 - GemAssetID: %s"), *GemAssetID.ToString());

	if (ResultIcon)
	{
		UAsyncDataManager* DataMgr = UAsyncDataManager::Get(this);
		if (!DataMgr)
		{
			UE_LOG(LogTemp, Error, TEXT("[CraftResult] AsyncDataManager가 null"));
			return;
		}

		// DA가 이미 로드되어 있으면 즉시 아이콘 표시
		UItemDataAsset* GemData = Cast<UItemDataAsset>(DataMgr->GetLoadedAsset(GemAssetID));
		if (GemData)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CraftResult] DA 이미 로드됨 → LoadIconFromDA 호출"));
			LoadIconFromDA(GemData);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[CraftResult] DA 미로드 → 비동기 로드 시작: %s"), *GemAssetID.ToString());
			// DA 자체가 아직 로드 안 됨 → 비동기 로드 후 아이콘 표시
			TArray<FPrimaryAssetId> IDs;
			IDs.Add(GemAssetID);

			FOnBundleLoadComplete OnLoaded;
			OnLoaded.BindLambda([this, DataMgr, GemAssetID]()
			{
				UE_LOG(LogTemp, Warning, TEXT("[CraftResult] 비동기 로드 콜백 도착 - GemAssetID: %s"), *GemAssetID.ToString());
				UItemDataAsset* LoadedGemData = Cast<UItemDataAsset>(DataMgr->GetLoadedAsset(GemAssetID));
				if (LoadedGemData)
				{
					UE_LOG(LogTemp, Warning, TEXT("[CraftResult] DA 로드 성공 → LoadIconFromDA 호출"));
					LoadIconFromDA(LoadedGemData);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[CraftResult] DA 로드 실패 - GetLoadedAsset null"));
				}
			});
			DataMgr->LoadAssetsByID(IDs, {}, OnLoaded);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[CraftResult] ResultIcon이 null"));
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
	return SoulGem.ToDescriptionString();
}

void UCraftResultWidget::LoadIconFromDA(UItemDataAsset* GemData)
{
	if (!GemData || !ResultIcon) return;

	UE_LOG(LogTemp, Warning, TEXT("[CraftResult] LoadIconFromDA - DA이름: %s, Icon null?: %d, Icon 경로: %s"),
		*GemData->GetName(), GemData->Icon.IsNull(), *GemData->Icon.ToSoftObjectPath().ToString());

	UTexture2D* IconTexture = GemData->Icon.Get();
	if (IconTexture)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CraftResult] 아이콘 즉시 로드 성공"));
		SetIcon(IconTexture);
	}
	else if (!GemData->Icon.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CraftResult] 아이콘 비동기 로드 시작"));
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

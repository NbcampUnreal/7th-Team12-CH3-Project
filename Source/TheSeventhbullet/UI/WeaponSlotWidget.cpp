#include "WeaponSlotWidget.h"
#include "Character/Component/EquipmentComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UWeaponSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SlotButton)
	{
		SlotButton->OnClicked.AddDynamic(this, &UWeaponSlotWidget::OnSlotButtonClicked);
	}
}

void UWeaponSlotWidget::NativeDestruct()
{
	if (StreamableHandle.IsValid())
	{
		StreamableHandle->ReleaseHandle();
		StreamableHandle.Reset();
	}
	Super::NativeDestruct();
}

void UWeaponSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (WeaponImage && WeaponTexture)
	{
		WeaponImage->SetBrushFromTexture(WeaponTexture);
	}
	
	if (WeaponName && !WeaponNameText.IsEmpty())
	{
		WeaponName->SetText(WeaponNameText);
	}
}

void UWeaponSlotWidget::InitSlot(AMainCharacter* InCharacter)
{
	OwningCharacter = InCharacter;
	
}
void UWeaponSlotWidget::OnSlotButtonClicked()
{
	if (!WeaponAssetId.IsValid()) return;
	
	UAssetManager& AssetManager = UAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(WeaponAssetId);
	
	if (!AssetPath.IsValid()) return;
	
	if (StreamableHandle.IsValid() && !StreamableHandle->HasLoadCompleted()) return;
	
	FStreamableManager& Streamable = AssetManager.GetStreamableManager();
	StreamableHandle = Streamable.RequestAsyncLoad(
		AssetPath,
		FStreamableDelegate::CreateWeakLambda(this, [this]()
		{
			OnWeaponDataLoaded();
		})
		);
	
}

void UWeaponSlotWidget::OnWeaponDataLoaded()
{
	UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(
		UAssetManager::Get().GetPrimaryAssetObject(WeaponAssetId)
		);
	
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponSlotWidget] DataAsset 로드 실패: %s"), *WeaponAssetId.ToString());
		return;
	}
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponSlotWidget] OwningCharacter가 없습니다."));
		return;
	}

	UEquipmentComponent* EquippedComp = OwningCharacter->FindComponentByClass<UEquipmentComponent>();
	if (!EquippedComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponSlotWidget] EquippedComponent를 찾을 수 없습니다."));
		return;
	}

	EquippedComp->SetPendingWeapon(WeaponData);
	OnSlotSelected.Broadcast(this);

	UE_LOG(LogTemp, Log, TEXT("[WeaponSlotWidget] 무기 장착 완료: %s"), *WeaponAssetId.ToString());
}

void UWeaponSlotWidget::SetSelected(bool bSelected)
{
	if (!SlotButton) return;

	if (bSelected)
	{
		SlotButton->SetBackgroundColor(FLinearColor(1.0f, 0.8f, 0.0f, 1.0f)); // 금색 강조
	}
	else
	{
		SlotButton->SetBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); // 기본 흰색
	}
}

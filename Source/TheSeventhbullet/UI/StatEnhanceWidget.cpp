#include "StatEnhanceWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Interaction/StatEnhancerComponent.h"
#include "Character/MainCharacter.h"
#include "Character/Component/StatusComponent.h"
#include "Manager/UIManager.h"
#include "UI/UITags.h"

void UStatEnhanceWidget::InitWidget(UStatEnhancerComponent* InComponent)
{
	CachedComponent = InComponent;
	RefreshAll();
}

void UStatEnhanceWidget::RefreshAll()
{
	if (!CachedComponent) return;

	AMainCharacter* Char = CachedComponent->GetCachedCharacter();
	if (Char && GoldText)
	{
		GoldText->SetText(
			FText::FromString(FString::Printf(TEXT("보유 골드: %d G"), Char->GetGold())));
	}

	RefreshStatRow(EStatusType::HP,      HP_LevelText,      HP_BonusText,      HP_CostText,      HP_UpgradeBtn);
	RefreshStatRow(EStatusType::ATK,     ATK_LevelText,     ATK_BonusText,     ATK_CostText,     ATK_UpgradeBtn);
	RefreshStatRow(EStatusType::DEF,     DEF_LevelText,     DEF_BonusText,     DEF_CostText,     DEF_UpgradeBtn);
	RefreshStatRow(EStatusType::Stamina, Stamina_LevelText, Stamina_BonusText, Stamina_CostText, Stamina_UpgradeBtn);

}

void UStatEnhanceWidget::OnClick_HP()
{
	TryUpgrade(EStatusType::HP);
}

void UStatEnhanceWidget::OnClick_ATK()
{
	TryUpgrade(EStatusType::ATK);
}

void UStatEnhanceWidget::OnClick_DEF()
{
	TryUpgrade(EStatusType::DEF);
}

void UStatEnhanceWidget::OnClick_Stamina()
{
	TryUpgrade(EStatusType::Stamina);
}

void UStatEnhanceWidget::OnClick_Close()
{
	if (UUIManager* UI = UUIManager::Get(this))
		UI->Close(UITags::StatEnhancer);
}

void UStatEnhanceWidget::RefreshStatRow(EStatusType StatType, UTextBlock* LevelText, UTextBlock* BonusText,
	UTextBlock* CostText, UButton* UpgradeBtn)
{
	if (!CachedComponent) return;

	AMainCharacter* Char = CachedComponent->GetCachedCharacter();
	if (!Char) return;

	UStatusComponent* StatusComp = Char->GetComponentByClass<UStatusComponent>();
	if (!StatusComp) return;

	// 현재 강화 레벨
	const FEnhancerStatus& ES = StatusComp->GetCharacterEnhanceStatus();
	int32 Level = 0;
	switch (StatType)
	{
	case EStatusType::HP:      Level = ES.EnhanceHp;      break;
	case EStatusType::ATK:     Level = ES.EnhanceAttack;   break;
	case EStatusType::DEF:     Level = ES.EnhanceDefense;  break;
	case EStatusType::Stamina: Level = ES.EnhanceStamina;  break;
	default: break;
	}

	const int32 Cost       = CachedComponent->GetUpgradeCost(StatType);
	const bool  bCanAfford = (Char->GetGold() >= Cost);

	if (LevelText)
		LevelText->SetText(FText::FromString(FString::Printf(TEXT("Lv.%d"), Level)));

	if (BonusText)
		BonusText->SetText(FText::FromString(GetBonusString(StatType, Level)));

	if (CostText)
	{
		CostText->SetText(FText::FromString(FString::Printf(TEXT("%d G"), Cost)));
		CostText->SetColorAndOpacity(
			bCanAfford
			? FSlateColor(FLinearColor(1.f,  0.85f, 0.2f,  1.f))   // 금색
			: FSlateColor(FLinearColor(0.9f, 0.2f,  0.15f, 1.f))); // 빨강
	}

	if (UpgradeBtn)
		UpgradeBtn->SetIsEnabled(bCanAfford);
}

void UStatEnhanceWidget::TryUpgrade(EStatusType StatType)
{
	if (!CachedComponent) return;
	CachedComponent->TryUpgradeStat(StatType);
}

FString UStatEnhanceWidget::GetBonusString(EStatusType StatType, int32 Level) const
{
	if (Level <= 0) return TEXT("+0");
	switch (StatType)
	{
	case EStatusType::HP:      return FString::Printf(TEXT("+%d HP"),  Level * FEnhancerIncreaseStatus::IncreaseHp);
	case EStatusType::ATK:     return FString::Printf(TEXT("+%d ATK"), Level * FEnhancerIncreaseStatus::IncreaseAttack);
	case EStatusType::DEF:     return FString::Printf(TEXT("+%d DEF"), Level * FEnhancerIncreaseStatus::IncreaseDefense);
	case EStatusType::Stamina: return FString::Printf(TEXT("+%d STA"), Level * FEnhancerIncreaseStatus::IncreaseStamina);
	default:                   return TEXT("+0");
	}
}

void UStatEnhanceWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 버튼 바인딩
	if (HP_UpgradeBtn)      HP_UpgradeBtn->OnClicked.AddDynamic(this,      &UStatEnhanceWidget::OnClick_HP);
	if (ATK_UpgradeBtn)     ATK_UpgradeBtn->OnClicked.AddDynamic(this,     &UStatEnhanceWidget::OnClick_ATK);
	if (DEF_UpgradeBtn)     DEF_UpgradeBtn->OnClicked.AddDynamic(this,     &UStatEnhanceWidget::OnClick_DEF);
	if (Stamina_UpgradeBtn) Stamina_UpgradeBtn->OnClicked.AddDynamic(this, &UStatEnhanceWidget::OnClick_Stamina);
	if (CloseButton)        CloseButton->OnClicked.AddDynamic(this,        &UStatEnhanceWidget::OnClick_Close);

	// 고정 텍스트
	if (TitleText)        TitleText->SetText(FText::FromString(TEXT("능력치 강화")));
	if (HP_NameText)      HP_NameText->SetText(FText::FromString(TEXT("HP")));
	if (ATK_NameText)     ATK_NameText->SetText(FText::FromString(TEXT("공격력")));
	if (DEF_NameText)     DEF_NameText->SetText(FText::FromString(TEXT("방어력")));
	if (Stamina_NameText) Stamina_NameText->SetText(FText::FromString(TEXT("스태미나")));

}

#include "StatEnhancerComponent.h"

#include "NetworkMessage.h"
#include "Character/Component/StatusComponent.h"

UStatEnhancerComponent::UStatEnhancerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CachedCharacter = nullptr;
}

int32 UStatEnhancerComponent::GetUpgradeCost(EStatusType StatType) const
{
	if (!CachedCharacter) return 0;
	UStatusComponent* StatusComponent = CachedCharacter->GetComponentByClass<UStatusComponent>();
	if (!StatusComponent) return 0;
	int32 CurrentLevel = 0;
	
	switch (StatType)
	{
	case EStatusType::HP:
		CurrentLevel = StatusComponent->GetCharacterEnhanceStatus().EnhanceHp;
		break;
	case EStatusType::ATK:
		CurrentLevel = StatusComponent->GetCharacterEnhanceStatus().EnhanceAttack;
		break;
	case EStatusType::DEF:
		CurrentLevel = StatusComponent->GetCharacterEnhanceStatus().EnhanceDefense;
		break;
	case EStatusType::Stamina:
		CurrentLevel = StatusComponent->GetCharacterEnhanceStatus().EnhanceStamina;
		break;
	default:
		UE_LOG(LogTemp,Error,TEXT("There is no Stat Type"));
		break;
	}
	
	return FMath::RoundToInt(1000.0f * FMath::Pow(1.1f, CurrentLevel));
}

bool UStatEnhancerComponent::TryUpgradeStat(EStatusType StatType)
{
	if (!CachedCharacter) return false;
	int32 Cost = GetUpgradeCost(StatType);
	
	UStatusComponent* StatusComponent = CachedCharacter->GetComponentByClass<UStatusComponent>();
	if (!StatusComponent) return false;
	FEnhancerStatus CurrentEnhanceStatus = StatusComponent->GetCharacterEnhanceStatus();
	if (CachedCharacter->GetGold() >= Cost)
	{
		CachedCharacter->AddGold(-Cost);
		switch (StatType)
		{
		case EStatusType::HP:
			CurrentEnhanceStatus.EnhanceHp++;
			break;
		case EStatusType::ATK:
			CurrentEnhanceStatus.EnhanceAttack++;
			break;
		case EStatusType::DEF:
			CurrentEnhanceStatus.EnhanceDefense++;
			break;
		case EStatusType::Stamina:
			CurrentEnhanceStatus.EnhanceStamina++;
			break;
		default:
			UE_LOG(LogTemp,Error,TEXT("There is no Stat Type"));
			break;
		}
		StatusComponent->SetCharacterEnhanceStatus(CurrentEnhanceStatus);
		return true;
	}
	return false;
}

void UStatEnhancerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatEnhancerComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);
	CachedCharacter = Cast<AMainCharacter>(Interactor);
	
	//TODO : UI띄우기??
}

void UStatEnhancerComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
	
}

void UStatEnhancerComponent::EndInteract(AActor* Interactor)
{
	Super::EndInteract(Interactor);
	CachedCharacter = nullptr;
}



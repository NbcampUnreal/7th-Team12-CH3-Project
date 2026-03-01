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
	const FEnhancerStatus& ES = StatusComponent->GetCharacterEnhanceStatus();
	int32 Level = 0;
	
	switch (StatType)
	{
	case EStatusType::HP:
		Level = StatusComponent->GetCharacterEnhanceStatus().EnhanceHp;
		break;
	case EStatusType::ATK:
		Level = StatusComponent->GetCharacterEnhanceStatus().EnhanceAttack;
		break;
	case EStatusType::DEF:
		Level = StatusComponent->GetCharacterEnhanceStatus().EnhanceDefense;
		break;
	case EStatusType::Stamina:
		Level = StatusComponent->GetCharacterEnhanceStatus().EnhanceStamina;
		break;
	default:
		UE_LOG(LogTemp,Error,TEXT("There is no Stat Type"));
		break;
	}
	
	return FMath::RoundToInt(1000.f * FMath::Pow(1.1f, static_cast<float>(Level)));
}

bool UStatEnhancerComponent::TryUpgradeStat(EStatusType StatType)
{
	if (!CachedCharacter) return false;

	UStatusComponent* StatusComp = CachedCharacter->GetComponentByClass<UStatusComponent>();
	if (!StatusComp) return false;

	const int32 Cost = GetUpgradeCost(StatType);
	if (CachedCharacter->GetGold() < Cost) return false;

	CachedCharacter->AddGold(-Cost);


	FEnhancerStatus ES = StatusComp->GetCharacterEnhanceStatus();
	switch (StatType)
	{
	case EStatusType::HP:      ES.EnhanceHp++;      break;
	case EStatusType::ATK:     ES.EnhanceAttack++;   break;
	case EStatusType::DEF:     ES.EnhanceDefense++;  break;
	case EStatusType::Stamina: ES.EnhanceStamina++;  break;
	default:
		UE_LOG(LogTemp, Error, TEXT("[StatEnhancer] TryUpgradeStat: Unknown StatType"));
		break;
	}
	StatusComp->SetCharacterEnhanceStatus(ES);

	OnStatUpgraded.Broadcast();
	return true;
}

void UStatEnhancerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatEnhancerComponent::BeginInteract(AActor* Interactor)
{
	CachedCharacter = Cast<AMainCharacter>(Interactor);
}

void UStatEnhancerComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
	
}

void UStatEnhancerComponent::EndInteract(AActor* Interactor)
{
	CachedCharacter = nullptr;
}



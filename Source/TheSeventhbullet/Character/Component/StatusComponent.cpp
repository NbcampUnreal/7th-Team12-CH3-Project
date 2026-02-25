#include "StatusComponent.h"

#include "EquipmentComponent.h"
#include "Character/MainCharacter.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CharacterStatus.Speed = 600.0f;
	CharacterStatus.HP = 100;
	CharacterStatus.Attack = 10;
	CharacterStatus.Defence = 10;
	CharacterStatus.CriticalChance = 15;
	CharacterStatus.CriticalDamage = 150;
}

void UStatusComponent::UpdateTotalStat()
{	
	UE_LOG(LogTemp, Warning, TEXT("Total Stat Updated"));
	
	FCharacterStat FinalStat;
	const FCharacterStat EnhanceStat = EnhanceStatToCharacterStat();
	FinalStat = EnhanceStat + CharacterStatus;
	
	UEquipmentComponent* GemComponent = GetOwner()->GetComponentByClass<UEquipmentComponent>();
	
	if (!GemComponent) return;
	FinalStat += GemComponent->GetTotalGemStats();
	
	AMainCharacter* Character = Cast<AMainCharacter>(GetOwner());
	if (!Character) return;
	
	Character->SetTotalStatus(FinalStat);
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();	
}

FCharacterStat UStatusComponent::EnhanceStatToCharacterStat()
{
	FCharacterStat FinalStat;
	
	FinalStat.HP = CharacterEnhanceStatus.EnhanceHp * FEnhancerIncreaseStatus::IncreaseHp;
	FinalStat.Defence = CharacterEnhanceStatus.EnhanceDefense * FEnhancerIncreaseStatus::IncreaseDefense;
	FinalStat.Attack = CharacterEnhanceStatus.EnhanceAttack * FEnhancerIncreaseStatus::IncreaseAttack;
	FinalStat.Stamina = CharacterEnhanceStatus.EnhanceStamina * FEnhancerIncreaseStatus::IncreaseStamina;
	
	return FinalStat;
}

FCharacterStat UStatusComponent::GetCharacterStatus() const
{
	return CharacterStatus;
}

void UStatusComponent::SetCharacterStatus(FCharacterStat& Status)
{
	this->CharacterStatus = Status;
}

void UStatusComponent::SetCharacterEnhanceStatus(FEnhancerStatus& Status)
{
	this->CharacterEnhanceStatus = Status;
}

FEnhancerStatus UStatusComponent::GetCharacterEnhanceStatus() const
{
	return CharacterEnhanceStatus;
}



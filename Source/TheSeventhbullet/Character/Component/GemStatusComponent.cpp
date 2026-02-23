#include "GemStatusComponent.h"
#include "Data/StatusTypes.h"
#include "DSP/AudioDebuggingUtilities.h"

// 장착된 소울젬에서 수집된 값들이 어떤 타입이냐에 따라 추가할 스탯이 어떤 스탯인지 판별하는 과정.
static float& GetStatusRef(FDynamicStatusValue& Status, EStatusType Type)
{
	switch (Type)
	{
	case EStatusType::HP: return Status.Dynamic_HP;
	case EStatusType::Stamina: return Status.Dynamic_Stamina;
	case EStatusType::ATK: return Status.Dynamic_ATK;
	case EStatusType::DEF: return Status.Dynamic_DEF;
	case EStatusType::DodgeCost: return Status.Dynamic_DodgeCost;
	case EStatusType::MaxSpeed: return Status.Dynamic_MaxSpeed;
	case EStatusType::SprintMultiplier: return Status.Dynamic_SprintMultiplier;
	case EStatusType::CritChance: return Status.Dynamic_CritChance;
	case EStatusType::CritDamage: return Status.Dynamic_CritDamage;
	default: return Status.Nothing;
	}
}

// 실제 스탯을 추가하는 부분.
void UGemStatusComponent::CalculateStatusFromModifiers(const TArray<FStatusModifier>& Modifiers)
{
	FinalStatus = BaseStatus;
	
	for (const FStatusModifier& Mod : Modifiers)
	{
		if (Mod.CalculationMethod == EStatusCalculationMethod::AddFlat)
		{
			GetStatusRef(FinalStatus, Mod.Status) += Mod.Value;
		}
		else if (Mod.CalculationMethod == EStatusCalculationMethod::Multiply)
		{
			GetStatusRef(FinalStatus, Mod.Status) *= Mod.Value;
		}
		UE_LOG(LogTemp, Warning, TEXT("CalculateStatusFromModifiers"));
	}
	
	// 크리티컬 관련 제한
	FinalStatus.Dynamic_CritChance = FMath::Clamp(FinalStatus.Dynamic_CritChance, 0.0f, 0.85f); // 최소 0.0f(0%), 최대 1.0f(100%)로 제한
	FinalStatus.Dynamic_CritDamage = FMath::Max(1.0f, FinalStatus.Dynamic_CritDamage); // 1.0f(100%)로 최솟값 제한
}

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GemStatusComponent.generated.h"

struct FStatusModifier;

USTRUCT(BlueprintType)
struct THESEVENTHBULLET_API FDynamicStatusValue
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_HP = 0.f; // 추가 HP
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_Stamina = 0.f; // 추가 Stamina
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_ATK = 0.f; // 추가 ATK
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_DEF = 0.f; // 추가 DEF
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_DodgeCost = 0.f; // 추가 DodgeCost
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_MaxSpeed = 0.f; // 추가 MaxSpeed
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_SprintMultiplier = 0.f; // 추가 SprintMultiplier
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_CritChance = 0.f; // 추가 크리티컬 확률. 최소 0.f(0%), 최대 0.85f(85%)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_CritDamage = 0.f; // 추가 크리티컬 데미지. 기본값 0.f 
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float Nothing = 0.0f; // 예외처리용 스탯.
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UGemStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	FDynamicStatusValue BaseStatus;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	FDynamicStatusValue FinalStatus;
	
	void CalculateStatusFromModifiers(const TArray<FStatusModifier>& Modifiers);
	
	// Get
	UFUNCTION(BlueprintCallable, Category = "Status|DamageMod")
	float GetFinalATK() const { return FinalStatus.Dynamic_ATK; }
	UFUNCTION(BlueprintCallable, Category = "Status|DamageMod")
	float GetFinalDEF() const { return FinalStatus.Dynamic_DEF; }
	UFUNCTION(BlueprintCallable, Category = "Status|DamageMod")
	float GetFinalCritChance() const { return FinalStatus.Dynamic_CritChance; }
	UFUNCTION(BlueprintCallable, Category = "Status|DamageMod")
	float GetFinalCritDamage() const { return FinalStatus.Dynamic_CritDamage; }
};

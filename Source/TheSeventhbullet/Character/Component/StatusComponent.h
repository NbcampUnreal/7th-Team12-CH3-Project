#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

struct FStatusModifier;

USTRUCT(BlueprintType)
struct THESEVENTHBULLET_API FDynamicStatusValue
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_HP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_Stamina = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_ATK = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_DEF = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_DodgeCost = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_MaxSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_SprintMultiplier = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_CritChance = 0.f; // 크리티컬 확률. 최소 0.f(0%), 최대 1.f(100%)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Dynamic_CritDamage = 1.5f; // 크리티컬 데미지. 기본값 1.5f(1.5배) 
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float Nothing = 0.0f; // 예외처리용 스탯.
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	FDynamicStatusValue BaseStatus;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	FDynamicStatusValue FinalStatus;
	
	void CalculateStatusFromModifiers(const TArray<FStatusModifier>& Modifiers);
	
	UFUNCTION(BlueprintCallable, Category = "Status|DamageMod")
	float GetFinalATK() const { return FinalStatus.Dynamic_ATK; }
	UFUNCTION(BlueprintCallable, Category = "Status|DamageMod")
	float GetFinalDEF() const { return FinalStatus.Dynamic_DEF; }
	UFUNCTION(BlueprintCallable, Category = "Status|DamageMod")
	float GetFinalCritChance() const { return FinalStatus.Dynamic_CritChance; }
	UFUNCTION(BlueprintCallable, Category = "Status|DamageMod")
	float GetFinalCritDamage() const { return FinalStatus.Dynamic_CritDamage; }
};

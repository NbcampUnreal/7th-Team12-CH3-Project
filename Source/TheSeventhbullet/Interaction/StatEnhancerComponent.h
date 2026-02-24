#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "Character/MainCharacter.h"
#include "Data/StatusTypes.h"
#include "StatEnhancerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UStatEnhancerComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	UStatEnhancerComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Enhance")
	int32 GetUpgradeCost(EStatusType StatType) const;
	
	//Return True : 돈이 있어서 stat 구매, Return False : 돈이 부족해서 Stat을 못삼
	UFUNCTION(BlueprintCallable, Category = "Enhance")
	bool TryUpgradeStat(EStatusType StatType);
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void BeginInteract(AActor* Interactor);
	virtual void ProgressInteract(AActor* Interactor);
	virtual void EndInteract(AActor* Interactor);
protected:
	UPROPERTY()
	AMainCharacter* CachedCharacter;
	
	FEnhancerIncreaseStatus IncreaseStat;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "GambleComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionResult, bool, bIsWin);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UGambleComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	UGambleComponent();

protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractionResult OnInteractionResult;
	virtual void BeginInteract(AActor* Interactor) override;
	virtual void ProgressInteract(AActor* Interactor) override;
	virtual void EndInteract(AActor* Interactor) override;
	
};

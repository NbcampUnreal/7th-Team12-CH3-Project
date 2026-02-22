// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseInteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UBaseInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBaseInteractionComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void BeginInteract(AActor* Interactor);
	virtual void ProgressInteract(AActor* Interactor);
	virtual void EndInteract(AActor* Interactor);
};

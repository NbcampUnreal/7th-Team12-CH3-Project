// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE()
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class THESEVENTHBULLET_API IInteractableInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Interactor) = 0; // 플레이어가 호출할 함수
};

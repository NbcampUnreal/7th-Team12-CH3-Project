// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "PlayerEnvQueryContext.generated.h"

/**
 * 
 */
UCLASS()
class THESEVENTHBULLET_API UPlayerEnvQueryContext : public UEnvQueryContext
{
	GENERATED_BODY()
	
protected:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};

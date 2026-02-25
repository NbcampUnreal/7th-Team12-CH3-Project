// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerEnvQueryContext.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"

void UPlayerEnvQueryContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* QueryOwner=Cast<AActor>(QueryInstance.Owner.Get());
	AActor* PlayerActor= UGameplayStatics::GetPlayerPawn(this, 0);
	
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerActor);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"

#include "Kismet/GameplayStatics.h"

UMainGameInstance* UMainGameInstance::Get(const UObject* WorldContext)
{
	return Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}

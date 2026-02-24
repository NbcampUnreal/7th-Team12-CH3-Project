// Fill out your copyright notice in the Description page of Project Settings.


#include "GambleComponent.h"
#include "Character/MainCharacter.h"
#include "Character/Component/EquipmentComponent.h"

UGambleComponent::UGambleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UGambleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGambleComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);

	
}

void UGambleComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);

	AMainCharacter* MainCharacter = Cast<AMainCharacter>(Interactor);
	if (!MainCharacter) return;
	
	UEquipmentComponent* EquipmentComponent = MainCharacter->FindComponentByClass<UEquipmentComponent>();
	if (!EquipmentComponent) return;
	
	const TArray<FSoulGemInstance>& SoulGems = EquipmentComponent->EquippedSoulGems;
	
	FGambleResultData ResultData;
	
	for (auto& Gems : SoulGems)
	{
		ResultData.TotalGrade += Gems.Grade;
		
		int32 RandAdd = FMath::RandRange(0,1);
		if (RandAdd == 1)
		{
			ResultData.PlayerFinalGrade += Gems.Grade;
			ResultData.bGemSuccessList.Add(true);
		}
		else
		{
			ResultData.bGemSuccessList.Add(false);
		}
	}
	
	ResultData.TargetGrade = FMath::RandRange(ResultData.TotalGrade - 5, ResultData.TotalGrade + 5);
	
	ResultData.bIsWin = (ResultData.PlayerFinalGrade >= ResultData.TargetGrade);
	
	OnInteractionResult.Broadcast(ResultData);
	
}

void UGambleComponent::EndInteract(AActor* Interactor)
{
	
	Super::EndInteract(Interactor);
}





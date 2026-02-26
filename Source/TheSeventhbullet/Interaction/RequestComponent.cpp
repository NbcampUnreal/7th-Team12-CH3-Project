#include "RequestComponent.h"

#include "Manager/SyncDataManager.h"
#include "Manager/UIManager.h"
#include "UI/RequestWidget.h"
#include "UI/UITags.h"
#include "System/MainGameMode.h"
#include "System/GameInstance/MainGameInstance.h"

URequestComponent::URequestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	
}

void URequestComponent::SelectRequest(int32 SelectedRequestID)
{
	EndInteract(GetOwner());

	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;

	GM->SetTargetRequestID(SelectedRequestID);
}

void URequestComponent::BeginInteract(AActor* Interactor)
{
	ProgressInteract(Interactor);
}

void URequestComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
	
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return;

	TArray<int32> AllRequestIDs = DataManager->GetAllRequestIDs();

	TArray<int32> ValidRequestIDs;
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	
	if (!GI) return;

	int32 CurrentDay = GI->CurrentDay;

	for (int32 RequestID : AllRequestIDs)
	{
		FRequestRowData Data = DataManager->GetRequestData(RequestID);
		if (DayAvailableLevel[CurrentDay] >= Data.RequestLevel)
		{
			ValidRequestIDs.Add(RequestID);
		}
	}

	if (ValidRequestIDs.Num() == 0) return;

	// 셔플
	for (int32 i = ValidRequestIDs.Num() - 1; i > 0; --i)
	{
		int32 RandomIndex = FMath::RandRange(0, i);
		ValidRequestIDs.Swap(i, RandomIndex);
	}

	int32 PickCount = FMath::Min(3, ValidRequestIDs.Num());
	TArray<int32> PickedIDs;
	TArray<FRequestRowData> SelectedRequests;
	for (int32 i = 0; i < PickCount; ++i)
	{
		int32 PickedID = ValidRequestIDs[i];
		PickedIDs.Add(PickedID);
		SelectedRequests.Add(DataManager->GetRequestData(PickedID));
	}

	// UI 표시
	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr) return;

	UUserWidget* Widget = UIMgr->PushByTag(UITags::Request);
	URequestWidget* RequestWidget = Cast<URequestWidget>(Widget);
	if (RequestWidget)
	{
		RequestWidget->SetRequests(this, PickedIDs, SelectedRequests);
	}
}

void URequestComponent::EndInteract(AActor* Interactor)
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Pop();
	}
}


void URequestComponent::BeginPlay()
{
	Super::BeginPlay();
	
}




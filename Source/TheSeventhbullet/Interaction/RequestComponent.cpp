#include "RequestComponent.h"

#include "Manager/SyncDataManager.h"
#include "Manager/UIManager.h"
#include "UI/RequestWidget.h"
#include "UI/UITags.h"
#include "System/MainGameMode.h"
#include "System/TownPhase.h"
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
	GM->SetTownPhase(ETownPhase::RequestAccepted);
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

    UMainGameInstance* GI = UMainGameInstance::Get(this);
    if (!GI) return;

    int32 CurrentDay = GI->CurrentDay;

    constexpr int32 BossRequestID = 100;
    constexpr int32 BossDay = 7;

    TArray<int32> AllRequestIDs = DataManager->GetAllRequestIDs();
    TArray<int32> PickedIDs;
    TArray<FRequestRowData> SelectedRequests;
	
    if (CurrentDay == BossDay)
    {
        FRequestRowData BossData = DataManager->GetRequestData(BossRequestID);
        PickedIDs.Add(BossRequestID);
        SelectedRequests.Add(BossData);
    }
    else
    {
        int32 DayIndex = CurrentDay - 1;
        if (!DayAvailableLevel.IsValidIndex(DayIndex)) return;
        int32 MaxLevel = DayAvailableLevel[DayIndex];

        TArray<int32> ValidRequestIDs;
        for (int32 RequestID : AllRequestIDs)
        {
            if (RequestID == BossRequestID) continue;

            FRequestRowData Data = DataManager->GetRequestData(RequestID);
            if (Data.RequestLevel <= MaxLevel)
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
        for (int32 i = 0; i < PickCount; ++i)
        {
            int32 PickedID = ValidRequestIDs[i];
            PickedIDs.Add(PickedID);
            SelectedRequests.Add(DataManager->GetRequestData(PickedID));
        }
    }

    if (PickedIDs.Num() == 0) return;

    // UI 표시
    UUIManager* UIMgr = UUIManager::Get(this);
    if (!UIMgr) return;

    UUserWidget* Widget = UIMgr->Open(UITags::Request);
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
		UIMgr->Close(UITags::Request);
	}
}


void URequestComponent::BeginPlay()
{
	Super::BeginPlay();
	
}




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
	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr) return;

	// 열려있으면 닫기
	if (UIMgr->IsOpen(UITags::Request))
	{
		UIMgr->Close(UITags::Request);
		return;
	}

	// 일차가 바뀌었으면 의뢰 갱신
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (!GI) return;

	if (LastRefreshedDay != GI->CurrentDay)
	{
		RefreshRequests();
		LastRefreshedDay = GI->CurrentDay;
	}

	if (CachedRequestIDs.Num() == 0) return;

	// 열기 + 캐시된 의뢰 세팅
	UUserWidget* Widget = UIMgr->Open(UITags::Request);
	URequestWidget* RequestWidget = Cast<URequestWidget>(Widget);
	if (RequestWidget)
	{
		RequestWidget->SetRequests(this, CachedRequestIDs, CachedRequests);
	}
}

void URequestComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
}

void URequestComponent::RefreshRequests()
{
	CachedRequestIDs.Empty();
	CachedRequests.Empty();

	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return;

	UMainGameInstance* GI = UMainGameInstance::Get(this);
	if (!GI) return;

	int32 CurrentDay = GI->CurrentDay;

	constexpr int32 BossRequestID = 100;
	constexpr int32 BossDay = 7;

	TArray<int32> AllRequestIDs = DataManager->GetAllRequestIDs();

	if (CurrentDay == BossDay)
	{
		FRequestRowData BossData = DataManager->GetRequestData(BossRequestID);
		CachedRequestIDs.Add(BossRequestID);
		CachedRequests.Add(BossData);
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
			CachedRequestIDs.Add(PickedID);
			CachedRequests.Add(DataManager->GetRequestData(PickedID));
		}
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




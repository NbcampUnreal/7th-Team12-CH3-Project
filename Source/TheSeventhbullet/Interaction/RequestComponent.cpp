#include "RequestComponent.h"

#include "Manager/SyncDataManager.h"
#include "System/MainGameMode.h"
#include "System/GameInstance/MainGameInstance.h"

URequestComponent::URequestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	
}

void URequestComponent::SelectRequest(int32 SelectedStageIndex)
{
	//TODO : UI에서 선택한 의뢰를 GameMode로 넘겨주는 작업
	
	EndInteract(GetOwner());
	
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (!GM) return;
	
	GM->SetTargetStageIndex(SelectedStageIndex);
	UE_LOG(LogTemp, Warning, TEXT("Quest Accepted!"));
}

void URequestComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);
}

void URequestComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
	
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager) return;
	
	int32 TotalCount = DataManager->GetTotalRequestCount();
	TArray<int32> ValidStageIndices;
	UMainGameInstance* GI = UMainGameInstance::Get(this);
	
	if (!GI) return;
	int32 CurrentDay = GI->CurrentDay;
	
	for (int32 i = 0 ; i < TotalCount; i++)
	{
		FRequestRowData Data = DataManager->GetStageData(i);
		if (DayAvailableLevel[CurrentDay] >= Data.RequestLevel)
		{
			ValidStageIndices.Add(i);
		}
	}
	
	if (ValidStageIndices.Num() == 0) return;
	
	//셔플
	for (int32 i = ValidStageIndices.Num() - 1; i > 0; --i)
	{
		int32 RandomIndex = FMath::RandRange(0,i);
		ValidStageIndices.Swap(i, RandomIndex);
	}
	
	int32 PickCount = FMath::Min(3,ValidStageIndices.Num());
	TArray<FRequestRowData> SelectedRequests;
	for (int32 i = 0 ; i < PickCount ; ++i)
	{
		int32 PickedIndex = ValidStageIndices[i];
		SelectedRequests.Add(DataManager->GetStageData(PickedIndex));
		//TODO : UI위젯에서 나중에 SelectRequest()를 부를 때 이 PickedIndex를 넘겨주도록해야함
	}
	
	//TODO : UI 띄우기 
}

void URequestComponent::EndInteract(AActor* Interactor)
{
	//TODO : UI 끄기
	Super::EndInteract(Interactor);
}


void URequestComponent::BeginPlay()
{
	Super::BeginPlay();
	
}




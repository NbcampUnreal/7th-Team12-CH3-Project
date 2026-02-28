#include "MaterialRecyclingComponent.h"

#include "CookerSettings.h"
#include "Inventory/ItemInstance.h"
#include "Manager/SyncDataManager.h"

UMaterialRecyclingComponent::UMaterialRecyclingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMaterialRecyclingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMaterialRecyclingComponent::BeginInteract(AActor* Interactor)
{
	Super::BeginInteract(Interactor);
}

void UMaterialRecyclingComponent::ProgressInteract(AActor* Interactor)
{
	Super::ProgressInteract(Interactor);
}

void UMaterialRecyclingComponent::EndInteract(AActor* Interactor)
{
	Super::EndInteract(Interactor);
}

bool UMaterialRecyclingComponent::AddMaterial(UMaterialDataAsset* Material)
{
	if (!Material)
	{
		return false;
	}
	if (RecyclingMaterials.Num() >= RecyclingMaterialCount)
	{
		return false;
	}
	
	RecyclingMaterials.Add(Material);
	BroadcastChanged();
	return true;
}

bool UMaterialRecyclingComponent::RemoveMaterial(int32 Index)
{
	if (!RecyclingMaterials.IsValidIndex(Index))
	{
		return false;
	}
	
	RecyclingMaterials.RemoveAt(Index);
	BroadcastChanged();
	return true;
}

void UMaterialRecyclingComponent::ClearMaterials()
{
	RecyclingMaterials.Reset();
	BroadcastChanged();
}

bool UMaterialRecyclingComponent::RecycleMaterials(FItemInstance& ResultMaterial)
{
	// Add한 Material의 수가 3보다 작으면 재활용 불가
	TArray<UMaterialDataAsset*> Materials;
	for (UMaterialDataAsset* Mat : RecyclingMaterials)
	{
		Materials.Add(Mat);
	}
	if (Materials.Num() < RecyclingMaterialCount)
	{
		return false;
	}
	
	// SyncManager에서 캐싱해둔 DataTable에 접근해야하므로 가져옴
	USyncDataManager* DataManager = USyncDataManager::Get(this);
	if (!DataManager)
	{
		return false;
	}
	
	// 재활용할 재료들의 평균 Grade를 구함
	const int32 BaseGrade = CalculateAvgGrade();
	if (BaseGrade <= 0)
	{
		return false;
	}
	
	// 구한 평균 Grade 기준으로 +0이 될지 +1이 될지 확률에 따라 계산해서 최종적으로 나오게 될 Material의 Grade를 확정
	const int32 ResultMaterialGrade = RollResultGrade(BaseGrade);
	
	// 확정한 최종Grade를 기준으로 캐싱해둔 DataTable에서 RecycleResultPool을 가져옴.
	const FMaterialRecycleRowData Row = DataManager->GetRecycleResultData(ResultMaterialGrade);
	if (Row.RecycleResultPool.Num() == 0)
	{
		return false;
	}
	
	// 가져온 RecycleResultPool에서 하나를 랜덤하게 결정
	TSoftObjectPtr<UMaterialDataAsset> PickedMaterial;
	if (!PickFromPool(Row.RecycleResultPool, PickedMaterial))
	{
		return false;
	}
	UMaterialDataAsset* PickedMaterialData = PickedMaterial.LoadSynchronous();
	if (!PickedMaterialData)
	{
		return false;
	}
	
	// 결정된 Material이 DataAsset이기 때문에 인벤토리에 넣기 위해서 ItemInstance를 생성
	ResultMaterial = FItemInstance(PickedMaterialData->GetPrimaryAssetId(), 1);
	if (!ResultMaterial.IsValid())
	{
		return false;
	}
	
	// 브로드캐스트
	OnMaterialRecycled.Broadcast(ResultMaterial);
	ClearMaterials();
	return true;
}

void UMaterialRecyclingComponent::BroadcastChanged() const
{
	TArray<UMaterialDataAsset*> Materials;
	for (UMaterialDataAsset* Mat : RecyclingMaterials)
	{
		Materials.Add(Mat);
	}
	OnRecyclingMaterialsChanged.Broadcast(Materials);
}

int32 UMaterialRecyclingComponent::CalculateAvgGrade() const
{
	int32 Sum = 0;
	int32 Count = 0;
	
	for (UMaterialDataAsset* Mat : RecyclingMaterials)
	{
		if (!Mat) continue;
		Sum += Mat->Grade;
		Count++;
	}
	
	if (Count <= 0)
	{
		return 0;
	}
	
	const float AvgGrade = static_cast<float>(Sum) / static_cast<float>(Count);
	// RoundToInt로 반올림해서 Grade의 평균을 int32 타입으로 만들었음. 이 때, 최대Grade(현재 6)를 넘지 않도록 Clamp.
	const int32 Grade = FMath::Clamp(FMath::RoundToInt(AvgGrade), 1, MaxGrade);
	return Grade;
}

int32 UMaterialRecyclingComponent::RollResultGrade(int32 Grade) const
{
	// PlusChance에 따라 확률적으로 Grade에 1을 더할지 말지를 결정하는 부분.
	const float Random = FMath::FRand();
	const int32 Plus = (Random < PlusChance) ? 1 : 0;
	
	return FMath::Clamp(Grade + Plus, 1, MaxGrade);
}

bool UMaterialRecyclingComponent::PickFromPool(const TArray<struct FMaterialDropEntry>& RecyclingPool,
	TSoftObjectPtr<UMaterialDataAsset>& PickedMaterial) const
{
	PickedMaterial = nullptr;
	
	if (RecyclingPool.Num() == 0)
	{
		return false;
	}
	
	// 가중치를 TotalWeight에 전부 더함
	float TotalWeight = 0.f;
	for (const FMaterialDropEntry& Entry : RecyclingPool)
	{
		TotalWeight += FMath::Max(0.f,Entry.DropWeight);
	}
	
	if (TotalWeight <= 0.f)
	{
		return false;
	}
	
	// 0 ~ TotalWeight 사이의 값을 랜덤으로 정함
	float Roll = FMath::RandRange(0.f, TotalWeight);
	
	// Roll 값에서 RecyclingPool에 있는 Material들의 가중치를 계속해서 빼고 그 결과가 0 이하가 되는 순간 해당 Material을 Pick함
	// 몬스터에게서 아이템을 드랍할 때 구현헀던 방식과 같은 방식
	for (const FMaterialDropEntry& Entry : RecyclingPool)
	{
		Roll -= FMath::Max(0.f, Entry.DropWeight);
		if (Roll <= 0.f)
		{
			PickedMaterial = Entry.Material;
			return PickedMaterial.ToSoftObjectPath().IsValid();
		}
	}
	
	PickedMaterial = RecyclingPool.Last().Material;
	return PickedMaterial.ToSoftObjectPath().IsValid();
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TheSeventhbullet/Data/TableRowTypes.h"
#include "SyncDataManager.generated.h"


/**
 * @brief 게임의 정적 데이터(Wave, Monster 등)를 전역 관리 및 캐싱하는 매니저 클래스
 * * @details
 * <b>1. 설계 의도 및 특징</b>
 * - <b>GameInstanceSubsystem 상속</b>:
 * - 게임 시작부터 종료까지 유지되는 수명 주기 (싱글톤 유사)
 * - 레벨(맵) 변경 시에도 데이터가 유지됨
 * * - <b>성능 최적화 (Caching)</b>:
 * - Initialize() 시점에 DataTable을 로드하여 TMap에 캐싱
 * - 템플릿 함수(LoadAndCacheTable)를 사용하여 코드 중복 방지
 * - 새로운 데이터 테이블 추가 시 로직 수정 최소화
 * * 
 * <b>2. 사용 예시 (Usage)</b>
 * @code
 * USyncDataManager* Manager = USyncDataManager::Get(GetWorld());
 * if (Manager)
 * {
 *		const FWaveRowData& WaveInfo = Manager->GetWaveData(1);//1은 SpawnList index
 * }
 * @endcode
 */

UCLASS()
class THESEVENTHBULLET_API USyncDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static USyncDataManager* Get(const UObject* WorldContext);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable, Category = "Data|Sync")
	FStageRowData GetStageData(int32 StageIndex) const;
	
	UFUNCTION(BlueprintPure, Category = "Data|Sync")
	int32 GetTotalWaveCount() const;
	
	UFUNCTION(BlueprintCallable, Category = "Data|Sync")
	FMonsterRowData GetMonsterData(const EMonsterType Tag) const;

	UFUNCTION(BlueprintCallable, Category = "Data|Sync")
	FWaveRowData GetWaveData(int32 StageIndex, int32 WaveIndex);
	
public:
	template<typename RowType, typename KeyType>
	static void LoadAndCacheTable(const TCHAR* Path, TMap<KeyType, RowType>& OutCache, TFunctionRef<KeyType(const RowType*)> KeySelector);
	
private:
	UPROPERTY()
	TMap<int32, FStageRowData> StageCache;
	
	UPROPERTY()
	TMap<EMonsterType, FMonsterRowData> MonsterCache;
	
};

#pragma region LoadHelper
// SoundManager가 생겨서 Load부분을 분리할지 고민중
// TODO : Wave, Monster, Sound 외의 다른 DataTable이 생기면 책임 분리 필요
// #pragma once
// #include "CoreMinimal.h"
// #include "Kismet/BlueprintFunctionLibrary.h"
// #include "Engine/DataTable.h"
// #include "DataTableHelper.generated.h"
//
// UCLASS()
// class THESEVENTHBULLET_API UDataTableHelper : public UBlueprintFunctionLibrary
// {
// 	GENERATED_BODY()
//
// public:
// 	// SyncDataManager에 있던 템플릿 함수를 static으로 가져옵니다.
// 	template <typename RowType, typename KeyType>
// 	static void LoadAndCacheTable(const TCHAR* Path, TMap<KeyType, RowType>& OutCache, TFunctionRef<KeyType(const RowType*)> KeySelector)
// 	{
// 		UDataTable* DataTable = LoadObject<UDataTable>(nullptr, Path);
// 		if (!DataTable)
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("[DataTableHelper] Failed to load DataTable at : %s"), Path);
// 			return;
// 		}
// 		
// 		FString ContextString;
// 		TArray<RowType*> AllRows;
// 		DataTable->GetAllRows(ContextString, AllRows);
// 		
// 		for (const RowType* Row : AllRows)
// 		{
// 			if (Row)
// 			{
// 				OutCache.Add(KeySelector(Row), *Row);
// 			}
// 		}
// 	}
// };
#pragma endregion

template <typename RowType, typename KeyType>
void USyncDataManager::LoadAndCacheTable(const TCHAR* Path, TMap<KeyType, RowType>& OutCache,
	TFunctionRef<KeyType(const RowType*)> KeySelector)
{
	UDataTable* DataTable = LoadObject<UDataTable>(nullptr,Path);
	if (!DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[SyncDataManager] Failed to load DataTable at : %s"),Path);
		return;
	}
	
	FString ContextString;
	TArray<RowType*> AllRows;
	DataTable->GetAllRows(ContextString, AllRows);
	for (const RowType* Row : AllRows)
	{
		if (Row)
		{
			OutCache.Add(KeySelector(Row), *Row);
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/WorldSubsystem.h"
#include "MonsterManagerSubSystem.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMonsterKilledDelegate);

class ASpawner;

USTRUCT()
struct FMonsterPoolArray
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<AActor*> Monsters;
};
/**
 * @brief [ Monster Logistics & Pooling Manager ]
 *
 * 몬스터의 생성, 보관(Pooling), 배치, 회수를 전담하는 월드 서브시스템입니다.
 *
 * * <b>1. 핵심 역할 (Core Roles)</b>
 * - <b>Object Pooling</b>: 몬스터를 계속 생성/파괴하는 대신, 미리 만들어두고 재사용하여 프레임 드랍을 방지합니다.
 * - <b>Resource Management</b>: GameMode의 요청에 따라 비동기 로딩을 수행하고 풀을 초기화(Warm-Up)합니다.
 * - <b>Lifecycle Control</b>: 몬스터의 활성화(Spawn)와 비활성화/반납(Return)을 관리합니다.
 *
 * * <b>2. 사용 예시 (Usage)</b>
 * @code
 * // 1. 스폰 (GameMode)
 * SubSystem->SpawnMonster(GoblinClass, RandomSpawnerIndex);
 *
 * // 2. 사망 처리 (Monster)
 * SubSystem->ReturnToPool(this);
 * @endcode
 */
UCLASS()
class THESEVENTHBULLET_API UMonsterManagerSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	static UMonsterManagerSubSystem* Get(const UObject* WorldContext);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	/**
	 * [월드 진입 시 초기화]
	 * 레벨에 배치된 모든 ASpawner 액터를 찾아 캐싱(CacheSpawners)합니다.
	 */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	/**
	 * [몬스터 회수 (Deactivate)]
	 * 몬스터가 사망하거나 사라져야 할 때 호출합니다.
	 * 액터를 Destroy하지 않고, 비활성화(Hidden, CollisionOff) 후 풀에 반납합니다.
	 * * 추가로 OnMonsterKilled 델리게이트를 방송하여 GameMode에 알립니다.
	 */
	void ReturnToPool(AActor* Monster);
	/**
	 * 현재 레벨에 있는 모든 ASpawner를 찾아 CachedSpawners 배열에 저장합니다.
	 * OnWorldBeginPlay에서 자동으로 호출됩니다.
	 */
	void CacheSpawners();
	/**
	 * [풀링 시스템 초기화 (Warm-Up)]
	 * GameMode(PrepareStage)로부터 몬스터 종류와 최대 수량을 전달받아,
	 * 1. 필요한 에셋을 비동기 로드하고
	 * 2. 로드가 완료되면 해당 수량만큼 액터를 미리 생성하여 풀을 채웁니다.
	 *
	 * @param Requirements { 몬스터 클래스(SoftPtr) : 필요한 최대 수량 }
	 * @param OnComplete 작업이 끝났을 때 GameMode에게 알릴 콜백
	 */
	void InitializePoolWithCounts(const TMap<TSoftClassPtr<AActor>, int32>& Requirements, FSimpleDelegate OnComplete);
	int32 GetCachedSpawnerCount();
	/**
	 * [몬스터 배치 (Activate)]
	 * 풀에서 몬스터를 하나 꺼내(Pop) 지정된 스포너 위치에 배치하고 활성화합니다.
	 * 만약 풀이 비어있다면(예외 상황), 새로 생성하여 배치합니다.
	 *
	 * @param MonsterAssetType 스폰할 몬스터의 클래스 정보
	 * @param SpawnPointIndex 배치할 스포너의 인덱스
	 */
	void SpawnMonster(TSoftClassPtr<AActor> MonsterAssetType, int32 SpawnPointIndex);
public:
	/** * 몬스터가 ReturnToPool을 통해 회수될 때 호출되는 이벤트입니다.
	 * MainGameMode가 이를 구독하여 AliveMonsterCount를 감소시킵니다.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMonsterKilledDelegate OnMonsterKilled;
private:
	/**
	 * 비동기 로딩이 완료된 후 호출되는 내부 콜백 함수입니다.
	 * 실제 액터 생성(SpawnActor)과 풀 채우기(Warm-Up)가 여기서 수행됩니다.
	 */
	void OnAssetsLoadedForPool(FSimpleDelegate OnComplete);
	/** 로딩 도중 데이터 손실 방지를 위한 임시 저장소 */
	TMap<TSoftClassPtr<AActor>, int32> PendingPoolRequirements;
private:
	UPROPERTY()
	TMap<UClass*, FMonsterPoolArray> MonsterPool;
	
	UPROPERTY()
	TArray<ASpawner*> CachedSpawners;
	
};


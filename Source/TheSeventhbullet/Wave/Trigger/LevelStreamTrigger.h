#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamTrigger.generated.h"

class UBoxComponent;
class ULoadingScreenWidget;

/**
 * 레벨 스트리밍 트리거 (순수 맵 전환용)
 *
 * [BP 설정 방법]
 *  1. 월드에 배치 후 OverlapVolume 크기 조절
 *  2. LevelToLoad   : 로드할 서브레벨 이름 (예: "L_Town")
 *  3. LevelToUnload : 언로드할 서브레벨 이름 (비워두면 언로드 안 함)
 *  4. bShowLoadingScreen : 로딩 화면 표시 여부
 *
 * [던전 진입이 필요한 경우]
 *  → ADungeonStreamTrigger 사용 (몬스터 풀 초기화 + 웨이브 시작 포함)
 */

UCLASS()
class THESEVENTHBULLET_API ALevelStreamTrigger : public AActor
{
	GENERATED_BODY()

public:
	ALevelStreamTrigger();
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	UBoxComponent* OverlapVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStream")
	FName LevelToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStream")
	FName LevelToUnload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStream")
	bool bShowLoadingScreen = true;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual bool CanActivate() const;
	virtual void HandleLevelLoaded();
	virtual void ActivateTrigger();
	void ShowLoadingScreen();
	void HideLoadingScreen();

	UPROPERTY()
	TObjectPtr<ULoadingScreenWidget> CachedLoadingWidget;

private:
	UFUNCTION()
	void OnLevelLoaded();

	bool bIsTriggered = false;
};

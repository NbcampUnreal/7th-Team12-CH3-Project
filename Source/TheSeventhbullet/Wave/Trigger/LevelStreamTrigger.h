#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamTrigger.generated.h"

class UBoxComponent;
class ULoadingScreenWidget;

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
	
	UPROPERTY(EditAnywhere, Category = "LevelStream")
	bool bIsDungeonEntrance = false;
	
private:
	UFUNCTION()
	void OnLevelLoaded();

	void ShowLoadingScreen();
	void HideLoadingScreen();

	UPROPERTY()
	TObjectPtr<ULoadingScreenWidget> CachedLoadingWidget;

	bool bIsTriggered = false;
};

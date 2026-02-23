////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * CrosshairWidget - 크로스헤어 + 히트마커
 *
 * UIManager::ShowByTag(UITags::Crosshair)로 표시.
 * 블루프린트에서 WBP_Crosshair를 만들고 BindWidget 위젯들을 배치해야 함.
 *
 * [블루프린트 구성 가이드]
 *   CanvasPanel (Fill Screen)
 *   ├── CrosshairImage  (중앙 앵커 0.5/0.5, Alignment 0.5/0.5)
 *   └── HitMarkerImage  (중앙 앵커 0.5/0.5, Alignment 0.5/0.5, 초기 Hidden)
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

class UImage;

UCLASS()
class THESEVENTHBULLET_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Crosshair")
	void SetCrosshairVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI|Crosshair")
	void ShowHitMarker();
	
	UPROPERTY(EditDefaultsOnly, Category = "UI|Crosshair")
	float HitMarkerDuration = 0.15f;

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CrosshairImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HitMarkerImage;

private:
	void HideHitMarker();

	FTimerHandle HitMarkerTimerHandle;
};
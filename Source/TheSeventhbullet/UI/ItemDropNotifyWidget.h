////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * ItemDropNotifyWidget - 아이템 드랍 알림 개별 엔트리
 *
 * 몬스터 처치 시 드랍된 아이템 1건의 아이콘/이름/수량을 표시하는 위젯.
 * MainHUDWidget의 DropNotifyBox에 자식으로 추가되어 사용됨.
 * 블루프린트에서 HideAnim(WidgetAnimation)을 만들면 사라질 때 애니메이션 재생.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/TableRowTypes.h"
#include "ItemDropNotifyWidget.generated.h"

DECLARE_DELEGATE(FOnHideAnimFinished);

class UImage;
class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UItemDropNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDropInfo(const FDroppedMaterialsData& DropData);

	// 사라지는 애니메이션 재생. 완료 시 OnHideFinished 호출.
	void PlayHide();

	FOnHideAnimFinished OnHideFinished;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCountText;

	// 블루프린트에서 "HideAnim" 이름으로 WidgetAnimation 생성 시 자동 바인딩
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> HideAnim;

private:
	UFUNCTION()
	void OnHideAnimComplete();
};

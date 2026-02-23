////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * UIDataAsset - FName Tag → WidgetClass 매핑 테이블
 *
 * 에디터에서 DA_UIConfig 에셋을 만들어 WidgetClassMap에 태그별 위젯 클래스를 등록.
 * UIManager가 Initialize에서 로드하여 Tag 기반 API에 활용.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Blueprint/UserWidget.h"
#include "UIDataAsset.generated.h"

UCLASS()
class THESEVENTHBULLET_API UUIDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TMap<FName, TSubclassOf<UUserWidget>> WidgetClassMap;

	TSubclassOf<UUserWidget> FindWidget(FName Tag) const
	{
		const TSubclassOf<UUserWidget>* Found = WidgetClassMap.Find(Tag);
		return Found ? *Found : nullptr;
	}
};

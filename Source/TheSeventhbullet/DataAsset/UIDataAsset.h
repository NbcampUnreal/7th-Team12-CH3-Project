#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Blueprint/UserWidget.h"
#include "TheSeventhbullet/UI/UITags.h"
#include "UIDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FUIWidgetEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EUILayer Layer = EUILayer::Game;
};

UCLASS()
class THESEVENTHBULLET_API UUIDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TMap<FName, FUIWidgetEntry> WidgetMap;

	const FUIWidgetEntry* FindEntry(FName Tag) const
	{
		return WidgetMap.Find(Tag);
	}

	TSubclassOf<UUserWidget> FindWidget(FName Tag) const
	{
		const FUIWidgetEntry* Entry = WidgetMap.Find(Tag);
		return Entry ? Entry->WidgetClass : nullptr;
	}
};

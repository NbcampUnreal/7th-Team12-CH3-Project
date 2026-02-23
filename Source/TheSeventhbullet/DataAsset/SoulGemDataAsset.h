#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "SoulGemDataAsset.generated.h"

UCLASS()
class THESEVENTHBULLET_API USoulGemDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetType GetItemType() const override
	{
		return FPrimaryAssetType("SoulGem");
	}
};

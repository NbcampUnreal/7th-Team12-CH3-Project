#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.generated.h"

USTRUCT(BlueprintType)
struct THESEVENTHBULLET_API FItemInstance
{
	GENERATED_BODY()

public:
	FItemInstance()
	   : ItemID(FPrimaryAssetId())
	   , StackCount(0)
	{}

	FItemInstance(FPrimaryAssetId InItemID, int32 InCount = 1)
	   : ItemID(InItemID)
	   , StackCount(InCount)
	{}

	bool IsValid() const
	{
		return ItemID.IsValid() && StackCount > 0;
	}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPrimaryAssetId ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackCount = 0;
};

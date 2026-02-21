#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "SoulGem/SoulGemProperties.h"
#include "MaterialDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMaterialTypes : uint8
{
	A_Attack,
	B_Utility,
	C_Amplification,
	D_Special
};

USTRUCT(BlueprintType)
struct THESEVENTHBULLET_API FGemNamingParts
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NamingParts")
	FText Prefix;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NamingParts")
	FText Suffix;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NamingParts")
	FText GemBaseName;
};

UCLASS()
class THESEVENTHBULLET_API UMaterialDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	EMaterialTypes MaterialType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	FGemNamingParts NamingParts;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	FStatusModifier StatusModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	ESpecialOptions SpecialOption = ESpecialOptions::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	int32 Grade = 1;
	
};

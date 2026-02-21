#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "MaterialDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMaterialType : uint8
{
	A_Attack,
	B_Utility,
	C_Amplification,
	D_Special
};

UENUM(BlueprintType)
enum class ESpecialOption : uint8
{
	None,
	ReloadSpeed,
	DoubleShot
};

USTRUCT(BlueprintType)
struct FGemNamingParts
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
	EMaterialType MaterialType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	FGemNamingParts NamingParts;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	ESpecialOption SpecialOption = ESpecialOption::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	int32 Grade = 1;
	
	
};

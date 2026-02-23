#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "SoulGem/SoulGemInstance.h"
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
	FText Prefix; // 접두사 ex) ~한
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NamingParts")
	FText Suffix; // 접미사 ex) ~의
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NamingParts")
	FText GemBaseName; // 베이스가 되는 이름(명사) ex) 탄환, 칼날, 뿔 등
};

UCLASS()
class THESEVENTHBULLET_API UMaterialDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	// 재료의 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	EMaterialTypes MaterialType;
	
	// 네이밍 규칙
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	FGemNamingParts NamingParts;
	
	// 스탯관련 옵션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	FStatusModifier StatusModifier;
	
	// 특수옵션 : 기본값 None
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	ESpecialOptions SpecialOption = ESpecialOptions::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaterialData")
	int32 Grade = 1; // 등급
};

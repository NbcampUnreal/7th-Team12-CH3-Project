#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberWidget.generated.h"

class UTextBlock;

UCLASS()
class THESEVENTHBULLET_API UDamageNumberWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDamageInfo(float Damage, bool bIsCrit);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;
};

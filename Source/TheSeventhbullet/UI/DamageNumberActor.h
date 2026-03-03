#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageNumberActor.generated.h"

class UWidgetComponent;
class UDamageNumberWidget;

UCLASS()
class THESEVENTHBULLET_API ADamageNumberActor : public AActor
{
	GENERATED_BODY()

public:
	ADamageNumberActor();

	void Init(float Damage, bool bIsCrit);

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> WidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	TSubclassOf<UUserWidget> DamageWidgetClass;

private:
	float ElapsedTime = 0.f;
	float LifeDuration = 1.0f;
	float FloatSpeed = 50.f;
};

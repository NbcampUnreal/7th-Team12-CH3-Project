#include "DamageNumberActor.h"
#include "DamageNumberWidget.h"
#include "Components/WidgetComponent.h"

ADamageNumberActor::ADamageNumberActor()
{
	PrimaryActorTick.bCanEverTick = true;

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	SetRootComponent(WidgetComp);

	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComp->SetDrawAtDesiredSize(true);
	WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADamageNumberActor::Init(float Damage, bool bIsCrit)
{
	if (DamageWidgetClass)
	{
		WidgetComp->SetWidgetClass(DamageWidgetClass);
		WidgetComp->InitWidget();
	}

	if (UDamageNumberWidget* DamageWidget = Cast<UDamageNumberWidget>(WidgetComp->GetWidget()))
	{
		DamageWidget->SetDamageInfo(Damage, bIsCrit);
	}

	if (bIsCrit)
	{
		SetActorScale3D(FVector(1.5f));
	}

	SetLifeSpan(LifeDuration);
}

void ADamageNumberActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	// Float upward
	FVector Location = GetActorLocation();
	Location.Z += FloatSpeed * DeltaTime;
	SetActorLocation(Location);

	// Fade out
	float Alpha = FMath::Clamp(1.0f - (ElapsedTime / LifeDuration), 0.0f, 1.0f);
	if (UDamageNumberWidget* DamageWidget = Cast<UDamageNumberWidget>(WidgetComp->GetWidget()))
	{
		DamageWidget->SetRenderOpacity(Alpha);
	}
}

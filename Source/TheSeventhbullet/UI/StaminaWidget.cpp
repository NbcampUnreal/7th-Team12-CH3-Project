#include "StaminaWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"

void UStaminaWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 다이나믹 머티리얼 생성
	if (StaminaFill && StaminaBarMaterial)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(StaminaBarMaterial, this);
		StaminaFill->SetBrushFromMaterial(DynMaterial);
	}

	// 캐릭터의 OnStaminaChanged 델리게이트 바인딩
	CachedCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (CachedCharacter)
	{
		CachedCharacter->OnStaminaChanged.AddDynamic(this, &UStaminaWidget::OnStaminaChanged);

		// 초기값 반영
		float Max = CachedCharacter->GetMaxStamina();
		float Percent = (Max > 0.f) ? CachedCharacter->GetCurrentStamina() / Max : 0.f;
		UpdatePercentage(Percent);
	}
}

void UStaminaWidget::NativeDestruct()
{
	if (CachedCharacter)
	{
		CachedCharacter->OnStaminaChanged.RemoveDynamic(this, &UStaminaWidget::OnStaminaChanged);
	}

	Super::NativeDestruct();
}

void UStaminaWidget::OnStaminaChanged(float CurrentStamina, float MaxStamina)
{
	float Percent = (MaxStamina > 0.f) ? CurrentStamina / MaxStamina : 0.f;
	UpdatePercentage(Percent);
}

void UStaminaWidget::UpdatePercentage(float Percent)
{
	if (DynMaterial)
	{
		DynMaterial->SetScalarParameterValue(FName("Percentage"), FMath::Clamp(Percent, 0.f, 1.f));
	}
}

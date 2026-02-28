#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveWidget.generated.h"

class UImage;
class UOverlay;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSleepComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWakeComplete);

UCLASS()
class THESEVENTHBULLET_API USaveWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Sleep")
	void PlaySleepAnimation();
	
	UFUNCTION(BlueprintCallable, Category = "Sleep")
	void PlayWakeAnimation();
	
	UPROPERTY(BlueprintAssignable, Category = "Sleep")
	FOnSleepComplete OnTransitionComplete_Sleep;

	UPROPERTY(BlueprintAssignable, Category = "Sleep")
	FOnWakeComplete OnTransitionComplete_Wake;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundOverlay;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ArchImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MoonImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SunImage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sleep|Textures")
	TObjectPtr<UTexture2D> ArchTexture;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sleep|Textures")
	TObjectPtr<UTexture2D> MoonTexture;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sleep|Textures")
	TObjectPtr<UTexture2D> SunTexture;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sleep|Timing")
	float TravelDuration = 2.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sleep|Timing")
	float FadeDuration = 1.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sleep|Layout")
	float PathYRatio = 0.45f;
	
private:
	enum class EPhase : uint8
	{
		Idle,
		Sleep_Travel,
		Sleep_FadeOut,
		Wake_Travel,
	};
	
	EPhase Phase = EPhase::Idle;
	float Elapsed = 0.f;
	
	void SetPhase(EPhase NewPhase);
	void TickSleepTravel(float Dt);
	void TickSleepFade(float Dt);
	void TickWakeTravel(float Dt);
	
	void MoveCelestial(UImage* Img, float Progress,  bool bReverse) const;
	void SetOverlayAlpha(float Alpha);
	
	float EaseInOut(float T) const { return T * T * (3.f - 2.f*T);}
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interaction/GambleComponent.h"
#include "GambleWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGambleFinished, bool, bIsWin);

UCLASS()
class THESEVENTHBULLET_API UGambleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Gamble")
	void StartGamble(const FGambleResultData& InData);
	
	UPROPERTY(BlueprintAssignable, Category = "Gamble")
	FOnGambleFinished OnGambleFinished;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TargetGradeText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerScoreText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ResultImage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gamble|Timing")
	float GemRevealInterval = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gambel|Timing")
	float ResultHoldDuration = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gamble|Visuals")
	TObjectPtr<UTexture2D> WinTexture;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gamble|Visuals")
	TObjectPtr<UTexture2D> LoseTexture;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gamble|Visuals")
	TSubclassOf<UUserWidget> GemSlotWidgetClass;
	
private:
	enum class EGamblePhase : uint8
	{
		Idle,
		Init, // 목표 점수 표시, 플레이어 점수 0 초기화
		GemReveal, // 젬 배열 순회하며 하나씩 공개
		ScoreCount, // PlayerFinalGrade까지 점수 카운트업
		ShowResult, // 승패 텍스트 / 이미지 표시
		HoldResult, // ResultHoldDuration 동안 대기
	};
	
	EGamblePhase Phase = EGamblePhase::Idle;
	float Elapsed = 0.f;
	
	FGambleResultData CachedData;
	int32 CurrentGemIndex = 0;
	int32 DisplayedScore = 0;
	
	void SetPhase(EGamblePhase NewPhase);
	
	void TickInit();
	void TickGemReveal();
	void TickScoreCount();
	void TickShowResult();
	void TickHoldResult();
	
	void UpdatePlayerScoreText(int32 Score);
	void ShowResult(bool bIsWin);
};

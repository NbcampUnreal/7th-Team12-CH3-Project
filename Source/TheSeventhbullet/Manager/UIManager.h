////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* [UIManager Guide]
 *
 * GameInstanceSubsystem - 위젯 캐싱 + Visibility 기반 Show/Hide, 스택(Push/Pop) 관리
 *
 * [핵심 원칙]
 *   - 위젯 인스턴스는 클래스당 1개, 캐시에서 영구 유지
 *   - AddToViewport는 최초 1회만, 이후 Visibility로만 전환
 *   - NativeConstruct/NativeDestruct는 최초 1회만 호출됨
 *
 * [사용 예시]
 *   UUIManager* UIMgr = UUIManager::Get(this);
 *
 *   // 1. HUD (항상 표시)
 *   UIMgr->ShowByTag(UITags::HUD);
 *   UIMgr->HideByTag(UITags::HUD);
 *
 *   // 2. 메뉴 (Tag 기반 토글)
 *   UIMgr->Toggle(UITags::Inventory);  // 열기/닫기 + 커서/입력모드 자동 처리
 *
 *   // 3. 기존 Class 기반 API도 그대로 사용 가능
 *   UIMgr->Push(UInventoryWidget::StaticClass());
 *   UIMgr->Pop();
 *   UIMgr->PopAll();
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "UIManager.generated.h"

class UUIDataAsset;

UCLASS()
class THESEVENTHBULLET_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UUIManager* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI")
	UUserWidget* GetWidget(TSubclassOf<UUserWidget> WidgetClass);

	// HUD (non-stack)
	UFUNCTION(BlueprintCallable, Category = "UI")
	UUserWidget* Show(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void Hide(TSubclassOf<UUserWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsVisible(TSubclassOf<UUserWidget> WidgetClass) const;

	// Menu UI (stack) - Visibility 기반
	UFUNCTION(BlueprintCallable, Category = "UI|Stack")
	UUserWidget* Push(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, Category = "UI|Stack")
	void Pop();

	UFUNCTION(BlueprintCallable, Category = "UI|Stack")
	void PopAll();

	// Tag 기반 API — UIDataAsset에서 FName → WidgetClass 자동 매핑
	UFUNCTION(BlueprintCallable, Category = "UI|Tag")
	UUserWidget* ShowByTag(FName Tag, int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, Category = "UI|Tag")
	void HideByTag(FName Tag);

	UFUNCTION(BlueprintCallable, Category = "UI|Tag")
	void Toggle(FName Tag, int32 ZOrder = 0);

private:
	TSubclassOf<UUserWidget> FindWidget(FName Tag) const;
	void UpdateInputModeForStack();

	UPROPERTY()
	TObjectPtr<UUIDataAsset> UIDataAsset;

	UPROPERTY()
	TMap<TSubclassOf<UUserWidget>, TObjectPtr<UUserWidget>> WidgetCache;

	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> WidgetStack;
};

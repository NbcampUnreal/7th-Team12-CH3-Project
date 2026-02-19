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
 *   UIMgr->Show(UHUDWidget::StaticClass());
 *   UIMgr->Hide(UHUDWidget::StaticClass());
 *
 *   // 2. 메뉴 (스택 기반)
 *   UIMgr->Push(UInventoryWidget::StaticClass());
 *   UIMgr->Push(UShopWidget::StaticClass());
 *   UIMgr->Pop();      // 상점 닫기 -> 인벤토리 복귀
 *   UIMgr->PopAll();   // 전부 닫기
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "UIManager.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "UI|Stack")
	bool IsTopOfStack(TSubclassOf<UUserWidget> WidgetClass) const;

	UFUNCTION(BlueprintPure, Category = "UI|Stack")
	bool IsStackEmpty() const { return WidgetStack.Num() == 0; }

private:
	UPROPERTY()
	TMap<TSubclassOf<UUserWidget>, TObjectPtr<UUserWidget>> WidgetCache;

	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> WidgetStack;
};

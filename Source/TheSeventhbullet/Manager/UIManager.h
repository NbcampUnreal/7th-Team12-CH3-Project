#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "TheSeventhbullet/UI/UITags.h"
#include "UIManager.generated.h"

class UUIDataAsset;
struct FUIWidgetEntry;

UCLASS()
class THESEVENTHBULLET_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UUIManager* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI")
	UUserWidget* Open(FName Tag);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void Close(FName Tag);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void Toggle(FName Tag);

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsOpen(FName Tag) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	UUserWidget* GetWidget(FName Tag);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HandleEscapeAction();

private:
	const FUIWidgetEntry* FindEntry(FName Tag) const;
	UUserWidget* GetOrCreateWidget(FName Tag, int32 ZOrder);
	void CloseWidget(UUserWidget* Widget, EUILayer Layer);
	void UpdateInputMode();
	void SetGameplayInputEnabled(bool bEnabled);

	static constexpr int32 ZOrder_Game     = 0;
	static constexpr int32 ZOrder_GameMenu = 10;
	static constexpr int32 ZOrder_Menu     = 20;
	static constexpr int32 ZOrder_Modal    = 30;

	UPROPERTY()
	TObjectPtr<UUIDataAsset> UIDataAsset;

	UPROPERTY()
	TMap<FName, TObjectPtr<UUserWidget>> WidgetCache;

	UPROPERTY()
	TObjectPtr<UUserWidget> ActiveGameMenu;
	FName ActiveGameMenuTag;

	UPROPERTY()
	TObjectPtr<UUserWidget> ActiveMenu;
	FName ActiveMenuTag;

	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> ModalStack;
	TArray<FName> ModalTagStack;
};

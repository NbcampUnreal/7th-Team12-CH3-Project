#include "UIManager.h"
#include "DataAsset/UIDataAsset.h"
#include "TheSeventhbullet/UI/UITags.h"
#include "TheSeventhbullet/Character/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const TCHAR* AssetPath = TEXT("/Game/TheSeventhBullet/Blueprints/UI/UIConfig/DA_UIConfig.DA_UIConfig");
	
	UIDataAsset = Cast<UUIDataAsset>(StaticLoadObject(UUIDataAsset::StaticClass(), nullptr, AssetPath));

	if (!UIDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIManager: DA_UIConfig not found"));
	}
}

void UUIManager::Deinitialize()
{
	WidgetStack.Empty();

	for (auto& Pair : WidgetCache)
	{
		if (Pair.Value && Pair.Value->IsInViewport())
		{
			Pair.Value->RemoveFromParent();
		}
	}
	WidgetCache.Empty();

	Super::Deinitialize();
}

UUIManager* UUIManager::Get(const UObject* WorldContextObject)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GI->GetSubsystem<UUIManager>();
	}
	return nullptr;
}

UUserWidget* UUIManager::GetWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass)
	{
		return nullptr;
	}

	if (TObjectPtr<UUserWidget>* Found = WidgetCache.Find(WidgetClass))
	{
		if (Found->Get())
		{
			return Found->Get();
		}
		WidgetCache.Remove(WidgetClass);
	}

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UUserWidget* NewWidget = CreateWidget<UUserWidget>(World, WidgetClass);
	if (NewWidget)
	{
		NewWidget->SetVisibility(ESlateVisibility::Collapsed);
		NewWidget->AddToViewport();
		WidgetCache.Add(WidgetClass, NewWidget);
	}
	return NewWidget;
}


UUserWidget* UUIManager::Show(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder)
{
	UUserWidget* Widget = GetWidget(WidgetClass);
	if (!Widget)
	{
		return nullptr;
	}

	Widget->SetVisibility(ESlateVisibility::Visible);
	return Widget;
}

void UUIManager::Hide(TSubclassOf<UUserWidget> WidgetClass)
{
	TObjectPtr<UUserWidget>* Found = WidgetCache.Find(WidgetClass);
	if (!Found || !Found->Get())
	{
		return;
	}

	Found->Get()->SetVisibility(ESlateVisibility::Collapsed);
}

bool UUIManager::IsVisible(TSubclassOf<UUserWidget> WidgetClass) const
{
	const TObjectPtr<UUserWidget>* Found = WidgetCache.Find(WidgetClass);
	if (!Found || !Found->Get())
	{
		return false;
	}
	return Found->Get()->GetVisibility() == ESlateVisibility::Visible;
}

UUserWidget* UUIManager::Push(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder)
{
	UUserWidget* Widget = GetWidget(WidgetClass);
	if (!Widget)
	{
		return nullptr;
	}

	// 이미 스택에 있으면 중복 Push 방지
	if (WidgetStack.Contains(Widget))
	{
		Widget->SetVisibility(ESlateVisibility::Visible);
		return Widget;
	}

	// 기존 Top 숨기기
	if (WidgetStack.Num() > 0)
	{
		UUserWidget* OldTop = WidgetStack.Last();
		if (OldTop)
		{
			OldTop->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	WidgetStack.Add(Widget);
	Widget->SetVisibility(ESlateVisibility::Visible);

	return Widget;
}

void UUIManager::Pop()
{
	if (WidgetStack.Num() == 0)
	{
		return;
	}

	UUserWidget* Top = WidgetStack.Pop();
	if (Top)
	{
		Top->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (WidgetStack.Num() > 0)
	{
		UUserWidget* NewTop = WidgetStack.Last();
		if (NewTop)
		{
			NewTop->SetVisibility(ESlateVisibility::Visible);
		}
	}

	UpdateInputModeForStack();
}

void UUIManager::PopAll()
{
	while (WidgetStack.Num() > 0)
	{
		UUserWidget* Top = WidgetStack.Pop();
		if (Top)
		{
			Top->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UpdateInputModeForStack();
}

// ──────────────────────────────────────────────
// Tag 기반 API
// ──────────────────────────────────────────────

TSubclassOf<UUserWidget> UUIManager::FindWidget(FName Tag) const
{
	if (!UIDataAsset)
	{
		return nullptr;
	}
	TSubclassOf<UUserWidget> WidgetClass = UIDataAsset->FindWidget(Tag);
	if (!WidgetClass)
	{	
		return nullptr;
	}
	return WidgetClass;
}

UUserWidget* UUIManager::ShowByTag(FName Tag, int32 ZOrder)
{
	return Show(FindWidget(Tag), ZOrder);
}

void UUIManager::HideByTag(FName Tag)
{
	Hide(FindWidget(Tag));
}

UUserWidget* UUIManager::PushByTag(FName Tag, int32 ZOrder)
{
	UUserWidget* Widget = Push(FindWidget(Tag), ZOrder);
	UpdateInputModeForStack();
	return Widget;
}

bool UUIManager::IsVisibleByTag(FName Tag) const
{
	return IsVisible(FindWidget(Tag));
}

void UUIManager::Toggle(FName Tag, int32 ZOrder)
{
	TSubclassOf<UUserWidget> WidgetClass = FindWidget(Tag);
	if (!WidgetClass)
	{
		return;
	}

	if (IsVisible(WidgetClass))
	{
		Pop();
	}
	else
	{
		Push(WidgetClass, ZOrder);
		UpdateInputModeForStack();
	}
}

void UUIManager::HandleEscapeAction()
{
	if (WidgetStack.Num() > 0)
	{
		Pop();
	}
	else
	{
		Toggle(UITags::EscMenu);
	}
}

void UUIManager::UpdateInputModeForStack()
{
	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	if (WidgetStack.Num() > 0)
	{
		PC->SetShowMouseCursor(true);
		SetGameplayInputEnabled(false);  // IMC_Gameplay 제거
	}
	else
	{
		PC->SetShowMouseCursor(false);
		SetGameplayInputEnabled(true);   // IMC_Gameplay 복원
	}

	bool bShouldPause = false;
	TSubclassOf<UUserWidget> EscMenuClass = FindWidget(UITags::EscMenu);
	if (EscMenuClass)
	{
		for (const TObjectPtr<UUserWidget>& Widget : WidgetStack)
		{
			if (Widget && Widget->GetClass() == EscMenuClass)
			{
				bShouldPause = true;
				break;
			}
		}
	}
	UGameplayStatics::SetGamePaused(World, bShouldPause);
}

void UUIManager::SetGameplayInputEnabled(bool bEnabled)
{
	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	if (!World) return;

	AMainPlayerController* PC = Cast<AMainPlayerController>(World->GetFirstPlayerController());
	if (!PC || !PC->InputMappingContext) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem) return;

	if (bEnabled)
	{
		if (!Subsystem->HasMappingContext(PC->InputMappingContext))
		{
			Subsystem->AddMappingContext(PC->InputMappingContext, 0);
		}
	}
	else
	{
		Subsystem->RemoveMappingContext(PC->InputMappingContext);
	}
}

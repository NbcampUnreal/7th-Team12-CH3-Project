#include "UIManager.h"
#include "DataAsset/UIDataAsset.h"
#include "TheSeventhbullet/Character/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const TCHAR* AssetPath = TEXT("/Game/TheSeventhBullet/Blueprints/UI/UIConfig/DA_UIConfig.DA_UIConfig");
	UIDataAsset = Cast<UUIDataAsset>(StaticLoadObject(UUIDataAsset::StaticClass(), nullptr, AssetPath));

	if (!UIDataAsset) return;
}

void UUIManager::Deinitialize()
{
	ActiveGameMenu = nullptr;
	ActiveMenu = nullptr;
	ModalStack.Empty();
	ModalTagStack.Empty();

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

const FUIWidgetEntry* UUIManager::FindEntry(FName Tag) const
{
	if (!UIDataAsset)
	{
		return nullptr;
	}
	return UIDataAsset->FindEntry(Tag);
}

UUserWidget* UUIManager::GetOrCreateWidget(FName Tag, int32 ZOrder)
{
	if (TObjectPtr<UUserWidget>* Found = WidgetCache.Find(Tag))
	{
		if (Found->Get())
		{
			return Found->Get();
		}
		WidgetCache.Remove(Tag);
	}

	const FUIWidgetEntry* Entry = FindEntry(Tag);
	if (!Entry || !Entry->WidgetClass)
	{
		return nullptr;
	}

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UUserWidget* NewWidget = CreateWidget<UUserWidget>(World, Entry->WidgetClass);
	if (NewWidget)
	{
		NewWidget->SetVisibility(ESlateVisibility::Collapsed);
		NewWidget->AddToViewport(ZOrder);
		WidgetCache.Add(Tag, NewWidget);
	}
	return NewWidget;
}

void UUIManager::CloseWidget(UUserWidget* Widget, EUILayer Layer)
{
	if (!Widget)
	{
		return;
	}
	Widget->SetVisibility(ESlateVisibility::Collapsed);
}

UUserWidget* UUIManager::Open(FName Tag)
{
	const FUIWidgetEntry* Entry = FindEntry(Tag);
	if (!Entry)
	{
		return nullptr;
	}

	const EUILayer Layer = Entry->Layer;
	int32 ZOrder = ZOrder_Game;

	switch (Layer)
	{
	case EUILayer::Game:
		ZOrder = ZOrder_Game;
		break;
	case EUILayer::GameMenu:
		ZOrder = ZOrder_GameMenu;
		break;
	case EUILayer::Menu:
		ZOrder = ZOrder_Menu;
		break;
	case EUILayer::Modal:
		ZOrder = ZOrder_Modal;
		break;
	}

	UUserWidget* Widget = GetOrCreateWidget(Tag, ZOrder);
	if (!Widget)
	{
		return nullptr;
	}

	switch (Layer)
	{
	case EUILayer::Game:
		Widget->SetVisibility(ESlateVisibility::Visible);
		break;

	case EUILayer::GameMenu:
		if (ActiveGameMenu && ActiveGameMenu != Widget)
		{
			return nullptr;
		}
		ActiveGameMenu = Widget;
		ActiveGameMenuTag = Tag;
		Widget->SetVisibility(ESlateVisibility::Visible);
		break;

	case EUILayer::Menu:
		if (ActiveMenu && ActiveMenu != Widget)
		{
			return nullptr;
		}
		ActiveMenu = Widget;
		ActiveMenuTag = Tag;
		Widget->SetVisibility(ESlateVisibility::Visible);
		break;

	case EUILayer::Modal:
		if (!ModalStack.Contains(Widget))
		{
			ModalStack.Add(Widget);
			ModalTagStack.Add(Tag);
		}
		Widget->SetVisibility(ESlateVisibility::Visible);
		break;
	}

	UpdateInputMode();
	return Widget;
}

void UUIManager::Close(FName Tag)
{
	const FUIWidgetEntry* Entry = FindEntry(Tag);
	if (!Entry)
	{
		return;
	}

	TObjectPtr<UUserWidget>* Found = WidgetCache.Find(Tag);
	if (!Found || !Found->Get())
	{
		return;
	}

	UUserWidget* Widget = Found->Get();
	const EUILayer Layer = Entry->Layer;

	switch (Layer)
	{
	case EUILayer::Game:
		Widget->SetVisibility(ESlateVisibility::Collapsed);
		break;

	case EUILayer::GameMenu:
		if (ActiveGameMenu == Widget)
		{
			CloseWidget(Widget, EUILayer::GameMenu);
			ActiveGameMenu = nullptr;
			ActiveGameMenuTag = NAME_None;
		}
		break;

	case EUILayer::Menu:
		if (ActiveMenu == Widget)
		{
			CloseWidget(Widget, EUILayer::Menu);
			ActiveMenu = nullptr;
			ActiveMenuTag = NAME_None;
		}
		break;

	case EUILayer::Modal:
		{
			int32 Idx = ModalStack.IndexOfByKey(Widget);
			if (Idx != INDEX_NONE)
			{
				CloseWidget(Widget, EUILayer::Modal);
				ModalStack.RemoveAt(Idx);
				ModalTagStack.RemoveAt(Idx);
			}
		}
		break;
	}

	UpdateInputMode();
}

void UUIManager::Toggle(FName Tag)
{
	if (IsOpen(Tag))
	{
		Close(Tag);
	}
	else
	{
		Open(Tag);
	}
}

bool UUIManager::IsOpen(FName Tag) const
{
	const TObjectPtr<UUserWidget>* Found = WidgetCache.Find(Tag);
	UE_LOG(LogTemp,Log,TEXT("IsOpen"));
	if (!Found || !Found->Get())
	{
		return false;
	}
	return Found->Get()->GetVisibility() == ESlateVisibility::Visible;
}

UUserWidget* UUIManager::GetWidget(FName Tag)
{
	const FUIWidgetEntry* Entry = FindEntry(Tag);
	if (!Entry)
	{
		return nullptr;
	}

	int32 ZOrder = ZOrder_Game;
	switch (Entry->Layer)
	{
	case EUILayer::GameMenu: ZOrder = ZOrder_GameMenu; break;
	case EUILayer::Menu:     ZOrder = ZOrder_Menu;     break;
	case EUILayer::Modal:    ZOrder = ZOrder_Modal;    break;
	default: break;
	}

	return GetOrCreateWidget(Tag, ZOrder);
}

void UUIManager::HandleEscapeAction()
{
	if (ModalStack.Num() > 0)
	{
		FName TopTag = ModalTagStack.Last();
		Close(TopTag);
		OnWidgetClosed.Broadcast(TopTag);
		return;
	}

	if (ActiveGameMenu)
	{
		FName ClosingTag = ActiveGameMenuTag;
		Close(ClosingTag);
		OnWidgetClosed.Broadcast(ClosingTag);
		return;
	}

	Toggle(UITags::EscMenu);
}

void UUIManager::UpdateInputMode()
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

	if (ModalStack.Num() > 0)
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		SetGameplayInputEnabled(false);
		UGameplayStatics::SetGamePaused(World, false);
	}
	else if (ActiveMenu)
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		SetGameplayInputEnabled(false);
		bool bShouldPause = (ActiveMenuTag == UITags::EscMenu);
		UGameplayStatics::SetGamePaused(World, bShouldPause);
	}
	else if (ActiveGameMenu)
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		SetGameplayInputEnabled(false);
		UGameplayStatics::SetGamePaused(World, false);
	}
	else
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
		SetGameplayInputEnabled(true);
		UGameplayStatics::SetGamePaused(World, false);
	}
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

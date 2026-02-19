#include "UIManager.h"
#include "Kismet/GameplayStatics.h"

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
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
}

bool UUIManager::IsTopOfStack(TSubclassOf<UUserWidget> WidgetClass) const
{
	if (WidgetStack.Num() == 0 || !WidgetClass)
	{
		return false;
	}
	return WidgetStack.Last()->IsA(WidgetClass);
}

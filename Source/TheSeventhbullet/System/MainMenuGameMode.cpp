#include "MainMenuGameMode.h"
#include "TheSeventhbullet/Manager/UIManager.h"
#include "TheSeventhbullet/UI/UITags.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	DefaultPawnClass = nullptr;
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}

	UUIManager* UIManager = UUIManager::Get(this);
	if (UIManager)
	{
		UIManager->ShowByTag(UITags::MainMenu);
	}
}

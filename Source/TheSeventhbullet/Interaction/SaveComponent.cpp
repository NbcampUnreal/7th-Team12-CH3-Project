#include "SaveComponent.h"
#include "UI/SaveWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

USaveComponent::USaveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaveComponent::BeginPlay()
{
	Super::BeginPlay();
	GI = UMainGameInstance::Get(this);
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && SaveWidgetClass)
	{
		SaveWidget = CreateWidget<USaveWidget>(PC, SaveWidgetClass);
		if (SaveWidget)
		{
			SaveWidget->AddToViewport(99); // 최상단 ZOrder
			SaveWidget->SetVisibility(ESlateVisibility::Hidden);

			SaveWidget->OnTransitionComplete_Sleep.AddDynamic(this, &USaveComponent::OnSleepAnimComplete);
			SaveWidget->OnTransitionComplete_Wake.AddDynamic(this, &USaveComponent::OnWakeAnimComplete);
		}
	}
}

void USaveComponent::OnSleepAnimComplete()
{
	HandleNextDay();

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
	{
		if (SaveWidget)
			SaveWidget->PlayWakeAnimation();
	}, 0.5f, false);
}

void USaveComponent::OnWakeAnimComplete()
{
	bIsTransitioning = false;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->ResetIgnoreMoveInput();
		PC->ResetIgnoreLookInput();
	}
}

void USaveComponent::HandleNextDay()
{
	GI->CurrentDay++;
}

void USaveComponent::SetPlayerInputEnabled(AActor* Interactor, bool bEnabled)
{
	APawn* PlayerPawn = Cast<APawn>(Interactor);
	if (!PlayerPawn) return;

	APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
	if (!PC) return;

	if (bEnabled)
	{
		PC->ResetIgnoreMoveInput();
		PC->ResetIgnoreLookInput();
	}
	else
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}
}

void USaveComponent::BeginInteract(AActor* Interactor)
{
	if (bIsTransitioning) return;

	if (!GI || !SaveWidget) return;

	bIsTransitioning = true;

	GI->SaveGameData();

	SetPlayerInputEnabled(Interactor, false);

	SaveWidget->PlaySleepAnimation();
}




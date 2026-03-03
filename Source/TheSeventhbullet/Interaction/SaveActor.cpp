#include "SaveActor.h"
#include "System/MainGameMode.h"
#include "System/TownPhase.h"


ASaveActor::ASaveActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SaveComponent = CreateDefaultSubobject<USaveComponent>(TEXT("SaveComponent"));
	PromptText = FText::FromString(TEXT("F키를 눌러 휴식"));
}

void ASaveActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASaveActor::Interact(AActor* Interactor)
{
	AMainGameMode* GM = AMainGameMode::Get(this);
	if (GM && !GM->CanTownInteract(ETownPhase::WaitForNextDay)) return;

	AMainCharacter* Character = Cast<AMainCharacter>(Interactor);
	if (!Character) return;

	if (SaveComponent)
		SaveComponent->BeginInteract(Interactor);
}


#include "BulletinActor.h"

#include "RequestComponent.h"
#include "Character/MainCharacter.h"

ABulletinActor::ABulletinActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RequestComp = CreateDefaultSubobject<URequestComponent>(TEXT("RequestComponent"));

	PromptText = FText::FromString(TEXT("F키를 눌러 의뢰 확인"));
}

void ABulletinActor::Interact(AActor* Interactor)
{
	AMainCharacter* Player = Cast<AMainCharacter>(Interactor);
	if (!Player) return;

	if (RequestComp)
	{
		RequestComp->BeginInteract(Interactor);
	}
}

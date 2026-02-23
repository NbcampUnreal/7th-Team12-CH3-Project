#include "CharacterSpawnPoint.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ACharacterSpawnPoint::ACharacterSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACharacterSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this,0);
	
	if (Character != nullptr)
	{
		Character->SetActorLocation(this->GetActorLocation());
		Character->SetActorRotation(this->GetActorRotation());
	}
}


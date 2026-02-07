// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheSeventhbulletGameMode.h"
#include "TheSeventhbulletCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATheSeventhbulletGameMode::ATheSeventhbulletGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

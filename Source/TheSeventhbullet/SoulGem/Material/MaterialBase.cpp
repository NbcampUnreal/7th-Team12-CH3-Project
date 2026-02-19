#include "MaterialBase.h"

AMaterialBase::AMaterialBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMaterialBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMaterialBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


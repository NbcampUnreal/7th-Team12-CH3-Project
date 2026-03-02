// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCharacter.h"
#include "TheSeventhbullet/Enemy/Boss/PatternComponent/BossPatternComponentBase.h"

// Sets default values
ABossCharacter::ABossCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BossPatternComponent=CreateDefaultSubobject<UBossPatternComponentBase>(FName("BossPatternComponent"));	
	
}


void ABossCharacter::SetPattern(UBossPatternDataAsset* PatternData)
{
	BossData=PatternData;
}

void ABossCharacter::PlayPattern(FName PatternName)
{
	
}




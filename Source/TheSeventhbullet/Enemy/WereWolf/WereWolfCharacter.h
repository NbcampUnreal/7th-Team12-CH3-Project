// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TheSeventhbullet/Enemy/EnemyBase.h"
#include "WereWolfCharacter.generated.h"
class AWereWolfAIController;

/**
 * WereWolf 캐릭터입니다.
 * 월드에 배치되거나 스폰될때 AIController에 빙의되게 만듭니다.
 */
UCLASS()
class THESEVENTHBULLET_API AWereWolfCharacter : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWereWolfCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	
};

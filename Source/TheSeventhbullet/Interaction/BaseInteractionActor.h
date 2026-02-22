// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "BaseInteractionActor.generated.h"

class UBoxComponent;
class UCameraComponent;
UCLASS()
class THESEVENTHBULLET_API ABaseInteractionActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ABaseInteractionActor();
	virtual void Interact(AActor* Interactor) override;
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlapComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componenets")
	TObjectPtr<UBaseInteractionComponent> InteractionComponent;
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Scene = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UCameraComponent> InteractCamera;
	
};

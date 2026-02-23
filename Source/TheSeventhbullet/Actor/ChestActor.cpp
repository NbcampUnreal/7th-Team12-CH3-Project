#include "ChestActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Inventory/InventoryComponent.h"
#include "TheSeventhbullet/Character/MainCharacter.h"

AChestActor::AChestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	SetRootComponent(ChestMesh);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBox"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetBoxExtent(FVector(150.f, 150.f, 150.f));
	InteractionVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	InteractionVolume->SetGenerateOverlapEvents(true);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetRelativeLocation(PromptOffset);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawAtDesiredSize(true);
	InteractionWidget->SetVisibility(false);

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AChestActor::OnOverlapBegin);
	InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AChestActor::OnOverlapEnd);
}

void AChestActor::Interact(AActor* Caller)
{
	if (!bPlayerInRange) return;

	// TODO: StorageWidget Push
}

void AChestActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
	if (Character)
	{
		bPlayerInRange = true;
		InteractionWidget->SetVisibility(true);
		//Character->SetCurrentInteractable(this);
	}
}

void AChestActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
	if (Character)
	{
		bPlayerInRange = false;
		InteractionWidget->SetVisibility(false);
		//Character->SetCurrentInteractable(nullptr);
	}
}

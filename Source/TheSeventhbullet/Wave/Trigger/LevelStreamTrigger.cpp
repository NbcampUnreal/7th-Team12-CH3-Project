#include "LevelStreamTrigger.h"

#include "Character/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/UIManager.h"
#include "UI/UITags.h"
#include "UI/LoadingScreenWidget.h"

ALevelStreamTrigger::ALevelStreamTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;

	OverlapVolume->SetCollisionProfileName(TEXT("Trigger"));
	OverlapVolume->SetBoxExtent(FVector(100.f, 100.f, 100.f));
}

void ALevelStreamTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (OverlapVolume)
	{
		OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamTrigger::OnOverlapBegin);
		OverlapVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FTimerHandle EnableTimerHandle;
		GetWorldTimerManager().SetTimer(EnableTimerHandle, [this]()
		{
			if (OverlapVolume)
			{
				OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}, 0.5f, false);
	}
}

void ALevelStreamTrigger::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(OtherActor);
	if (!PlayerCharacter || bIsTriggered || !CanActivate())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[LevelStreamTrigger] Overlap! Player: %s, Trigger: %s, LevelToLoad: %s"),
		*PlayerCharacter->GetActorLocation().ToString(),
		*GetActorLocation().ToString(),
		*LevelToLoad.ToString());

	bIsTriggered = true;

	if (bShowLoadingScreen)
	{
		ShowLoadingScreen();
	}

	if (LevelToLoad != NAME_None)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = GetUniqueID();

		UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, false, LatentInfo);
	}

	if (LevelToUnload != NAME_None)
	{
		FLatentActionInfo UnloadLatentInfo;
		UnloadLatentInfo.CallbackTarget = this;
		UnloadLatentInfo.ExecutionFunction = NAME_None;
		UnloadLatentInfo.Linkage = 1;
		UnloadLatentInfo.UUID = GetUniqueID() + 1;

		UGameplayStatics::UnloadStreamLevel(this, LevelToUnload, UnloadLatentInfo, false);
	}
}

bool ALevelStreamTrigger::CanActivate() const
{
	return true;
}

void ALevelStreamTrigger::OnLevelLoaded()
{
	bIsTriggered = false;
	HandleLevelLoaded();
}

void ALevelStreamTrigger::HandleLevelLoaded()
{
	HideLoadingScreen();
}

void ALevelStreamTrigger::ShowLoadingScreen()
{
	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UUserWidget* Widget = UIMgr->Open(UITags::LoadingScreen);
		CachedLoadingWidget = Cast<ULoadingScreenWidget>(Widget);
		if (CachedLoadingWidget)
		{
			CachedLoadingWidget->SetProgress(0.0f);
		}
	}
}

void ALevelStreamTrigger::HideLoadingScreen()
{
	if (CachedLoadingWidget)
	{
		CachedLoadingWidget->SetProgress(1.0f);
	}
	CachedLoadingWidget = nullptr;

	UUIManager* UIMgr = UUIManager::Get(this);
	if (UIMgr)
	{
		UIMgr->Close(UITags::LoadingScreen);
	}
}

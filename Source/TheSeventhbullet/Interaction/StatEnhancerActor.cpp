#include "StatEnhancerActor.h"

#include "StatEnhancerComponent.h"
#include "Manager/UIManager.h"


AStatEnhancerActor::AStatEnhancerActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StatEnhancerComponent = CreateDefaultSubobject<UStatEnhancerComponent>(TEXT("StatEnhancerComponent"));
	
	PromptText = FText::FromString(TEXT("F키를 눌러 능력치 강화"));
}

void AStatEnhancerActor::Interact(AActor* Interactor)
{
	if (!StatEnhancerComponent) return;
	
	if (!bIsUIOpen)
	{
		StatEnhancerComponent->BeginInteract(Interactor);
		OpenEnhancerUI();
	}
	else
	{
		StatEnhancerComponent->EndInteract(Interactor);
		CloseEnhancerUI();
	}
}

void AStatEnhancerActor::OpenEnhancerUI()
{
	UUIManager* UI = UUIManager::Get(this);
	if (!UI) return;
	
	UUserWidget* Raw = UI->Open(UITags::StatEnhancer);
	CachedWidget = Cast<UStatEnhanceWidget>(Raw);
	
	if (CachedWidget)
	{
		CachedWidget->InitWidget(StatEnhancerComponent);
	}
	bIsUIOpen = true;
}

void AStatEnhancerActor::CloseEnhancerUI()
{
	if (UUIManager* UI = UUIManager::Get(this))
		UI->Close(UITags::StatEnhancer);
	
	CachedWidget = nullptr;
	bIsUIOpen = false;
}

void AStatEnhancerActor::OnStatUpgraded()
{
	if (CachedWidget)
		CachedWidget->RefreshAll();
}

void AStatEnhancerActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (StatEnhancerComponent)
	{
		StatEnhancerComponent->OnStatUpgraded.AddDynamic(this, &AStatEnhancerActor::OnStatUpgraded);
	}
	
}


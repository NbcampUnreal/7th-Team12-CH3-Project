#include "StatEnhancerActor.h"

#include "StatEnhancerComponent.h"
#include "Character/Component/StatusComponent.h"
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

	UUIManager* UI = UUIManager::Get(this);
	if (!UI) return;

	if (!UI->IsOpen(UITags::StatEnhancer))
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
	UUIManager* UIMgr = UUIManager::Get(this);
	if (!UIMgr) return;
	UE_LOG(LogTemp,Log,TEXT("Enhancer Open!!!"));
	UUserWidget* Raw = UIMgr->Open(UITags::StatEnhancer);
	if (!Raw) return;

	CachedWidget = Cast<UStatEnhanceWidget>(Raw);
	if (CachedWidget)
	{
		CachedWidget->InitWidget(StatEnhancerComponent);
	}
}

void AStatEnhancerActor::CloseEnhancerUI()
{
	if (UUIManager* UI = UUIManager::Get(this))
		UI->Close(UITags::StatEnhancer);
	
	CachedWidget = nullptr;
}

void AStatEnhancerActor::OnStatUpgraded()
{
	if (StatEnhancerComponent)
		StatEnhancerComponent->GetCachedCharacter()->StatusComponent->UpdateTotalStat();
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


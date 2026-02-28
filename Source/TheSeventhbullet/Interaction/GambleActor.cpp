#include "GambleActor.h"

#include "Character/MainCharacter.h"
#include "Manager/UIManager.h"
#include "UI/GambleDialogueWidget.h"
#include "UI/GambleWidget.h"

AGambleActor::AGambleActor()
{
	PrimaryActorTick.bCanEverTick = false;
	GambleComponent = CreateDefaultSubobject<UGambleComponent>(TEXT("GambleComponent"));
	InteractionComponent = GambleComponent;
	
	PromptText = FText::FromString(TEXT("F키를 눌러 도박"));
}

void AGambleActor::Interact(AActor* Interactor)
{
	Super::Interact(Interactor);
	AMainCharacter* Player = Cast<AMainCharacter>(Interactor);
	if (!Player) return;
	
	CachedInteractor = Interactor;
	UUIManager* UI = UUIManager::Get(this);
	if (!UI) return;
	
	UUserWidget* RawWidget = UI->Open(UITags::GambleDialogue);
	if (UGambleDialogueWidget* DialogueWidget = Cast<UGambleDialogueWidget>(RawWidget))
	{
		// 중복 바인딩 방지 (안전장치)
		DialogueWidget->OnConfirmed.RemoveDynamic(this, &AGambleActor::OnDialogueConfirmed);
		DialogueWidget->OnCancelled.RemoveDynamic(this, &AGambleActor::OnDialogueCancelled);
		
		// 위젯의 버튼 이벤트와 액터의 함수를 연결!
		DialogueWidget->OnConfirmed.AddDynamic(this, &AGambleActor::OnDialogueConfirmed);
		DialogueWidget->OnCancelled.AddDynamic(this, &AGambleActor::OnDialogueCancelled);
	}
}

void AGambleActor::OnInteractionResult(const FGambleResultData& GambleData)
{
	UUIManager* UI = UUIManager::Get(this);
	if (!UI) return;
	
	UUserWidget* RawWidget = UI->Open(UITags::Gamble);
	UGambleWidget* GambleWidget = Cast<UGambleWidget>(RawWidget);
	if (!GambleWidget) return;
	
	GambleWidget->OnGambleFinished.AddDynamic(this, &AGambleActor::OnGambleFinished);
	GambleWidget->StartGamble(GambleData);
}

void AGambleActor::OnGambleFinished(bool bIsWin)
{
	UUIManager* UI = UUIManager::Get(this);
	if (UI)
	{
		if (UUserWidget* W = UI->GetWidget(UITags::Gamble))
		{
			if (UGambleWidget* GW = Cast<UGambleWidget>(W))
				GW->OnGambleFinished.RemoveDynamic(this, &AGambleActor::OnGambleFinished);
			UI->Close(UITags::Gamble);
		}
	}
	if (bIsWin)
		GiveReward();
	else
		ApplyPenalty();
	if (GambleComponent && CachedInteractor)
	{
		GambleComponent->EndInteract(CachedInteractor);
	}
}

void AGambleActor::OnDialogueConfirmed()
{
	UUIManager* UI = UUIManager::Get(this);
	if (UI) UI->Close(UITags::GambleDialogue);
	if (GambleComponent && CachedInteractor)
		GambleComponent->ProgressInteract(CachedInteractor);
}

void AGambleActor::OnDialogueCancelled()
{
	UUIManager* UI = UUIManager::Get(this);
	if (UI) UI->Close(UITags::GambleDialogue);
	if (GambleComponent && CachedInteractor)
	{
		GambleComponent->EndInteract(CachedInteractor);
	}
}

void AGambleActor::GiveReward()
{
	//TODO : 골드 획득
}

void AGambleActor::ApplyPenalty()
{
	//TODO : 골드 차감
}

void AGambleActor::BeginPlay()
{
	Super::BeginPlay();
	if (GambleComponent)
		GambleComponent->OnInteractionResult.AddDynamic(this, &AGambleActor::OnInteractionResult);
}


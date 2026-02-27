#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionActor.h"
#include "BulletinActor.generated.h"

class URequestComponent;

/**
 * 의뢰 게시판 액터
 *
 * [사용법]
 *  1. 월드에 배치 후 StaticMesh에 게시판 메시 설정
 *  2. TriggerBox 크기 조절 (플레이어 감지 범위)
 *  3. F키 상호작용 → 의뢰 목록 UI 표시 → 선택 시 던전 진입 준비
 *
 * [구조]
 *  ChestActor와 동일한 패턴 — Interact()를 override하여
 *  카메라 전환 없이 바로 RequestComponent의 의뢰 로직 실행
 */
UCLASS()
class THESEVENTHBULLET_API ABulletinActor : public ABaseInteractionActor
{
	GENERATED_BODY()

public:
	ABulletinActor();

	virtual void Interact(AActor* Interactor) override;

	UFUNCTION(BlueprintPure, Category = "Interaction|Request")
	URequestComponent* GetRequestComponent() const { return RequestComp; }

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Request")
	TObjectPtr<URequestComponent> RequestComp;
};

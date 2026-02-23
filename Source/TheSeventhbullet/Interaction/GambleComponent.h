// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractionComponent.h"
#include "GambleComponent.generated.h"

//UI 시나리오
//초기화 : UI에 목표 점수(TargetGrade)를 표시, 플레이어 점수는 0으로 둠
//배열 순회 
//점수 상승 : 배열의 값이 True일 때만 플레이어의 UI점수를 올림
//결과 표시 : 배열을 모두 순회하며 최종적으로 PlayerFinalGrade와 UI 점수가 일치하게끔
//이때, bIsWin값을 확인하여 승리 또는 패배 애니메이션을 띄움
//보상 지급 : UI 연출이 끝난 시점에 캐릭터에게 실제 보상을 주거나 아이템을 뺏는 함수 호출
USTRUCT(BlueprintType)
struct FGambleResultData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gamble")
	int32 TotalGrade = 0;// 보유한 소울젬의 총 등급 합
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gamble")
	int32 TargetGrade = 0;// 이겨야 하는 목표 점수 (RandGrade)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gamble")
	int32 PlayerFinalGrade = 0;//플레이어가 최종적으로 얻은 점수
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gamble")
	TArray<bool> bGemSuccessList;//각 소울젬이 점수 합산에 성공했는지 실패했는지 여부
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gamble")
	bool bIsWin = false;//최종 승패
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionResultSignature, const FGambleResultData&, GambleData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THESEVENTHBULLET_API UGambleComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

public:
	UGambleComponent();

protected:
	virtual void BeginPlay() override;
public:
	virtual void BeginInteract(AActor* Interactor) override;
	virtual void ProgressInteract(AActor* Interactor) override;
	virtual void EndInteract(AActor* Interactor) override;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionResultSignature OnInteractionResult;
};

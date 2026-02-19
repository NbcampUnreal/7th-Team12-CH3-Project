// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

#pragma region DELEGATE
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterEventSignnature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterSetAISignnature,
											UBehaviorTree*,ParamBT,
											float,AttackRadius);
#pragma endregion


/**
 * PDA를 통해 로드 완료된 데이터를 가져와서 적 캐릭터 정보를 들고 있고, 피격 등의 이벤트를 진행합니다.
 */
class UBehaviorTree;
UCLASS()
class THESEVENTHBULLET_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
#pragma region DELEGATE METHOD
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnCharacterHit;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnCharacterHeadHit;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnCharacterDead;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCharacterEventSignnature OnCharacterReset;
	
	//비헤이비어 트리 초기 세팅을 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category="Settings")
	FOnCharacterSetAISignnature OnCharacterSetAI;

	
#pragma endregion
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//몬스터를 해당 값으로 초기화합니다.
	UFUNCTION(BlueprintCallable,Category="Enemy|Status")
	void SetupEnemy(UEnemyDataAsset* LoadedData);
	//공격력을 반환합니다.
	float GetAttackPoint();
	
	//Projectile의 StaticMesh를 전달합니다.
	TObjectPtr<UStaticMesh> GetProjectileStaticMesh();
	//Projectile의 속도를 전달합니다.
	float GetProjectileSpeed();
	//Projectile의 호밍 여부를 전달합니다.
	bool GetbIsHoming();
	
	//오브젝트 풀에서 캐릭터를 생성할때 리셋해줍니다.
	UFUNCTION(BlueprintCallable,Category="Enemy|Status")
	void ResetEnemy();
	//PDA에서 받은 로딩된 애님몽타주를 실행합니다.
	UFUNCTION(BlueprintCallable,Category="Enemy|Status")
	UAnimMontage* ReturnthisMontage(FName AMName);

	
	UFUNCTION()
	EMonsterType GetMonsterType();
	
	UFUNCTION()
	void SetMonsterType(EMonsterType InEnemyMonsterType);
	

protected:
#pragma region EnemyStatus
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float NowHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float MaxHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float ArmorPoint;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float AttackPoint;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float KnockbackStrengh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Status")
	float AttackRadius;
#pragma endregion
	
#pragma  region EnemyData
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Hit")
	UParticleSystem* HitParticle;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy|Hit")
    UParticleSystem* HeadShotParticle;
	
	//가져온 PDA를 캐싱하기 위한 변수
	TObjectPtr<UEnemyDataAsset> EnemyData;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemy")
	EMonsterType EnemyMonsterType;

	
#pragma endregion
	
	
	UFUNCTION(BlueprintCallable)
	virtual void EnemyTakePointDamage(
	AActor* DamagedActor, 
	float Damage, 
	class AController* InstigatedBy, 
	FVector HitLocation, 
	class UPrimitiveComponent* FHitComponent, 
	FName BoneName, 
	FVector ShotFromDirection, 
	const class UDamageType* DamageType, 
	AActor* DamageCauser
	);
	
	
	void SetHealth(float NewHealth);
	void DisplayParticle(FVector HitLocation, UParticleSystem* InParticle);
	void ReturnToPool();

private:
	bool bIsDead;
	
};

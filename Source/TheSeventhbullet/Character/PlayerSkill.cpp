#include "PlayerSkill.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


APlayerSkill::APlayerSkill()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetSphereRadius(15.0f);
	SetRootComponent(Collision);
	
	Collision->OnComponentHit.AddDynamic(this, &APlayerSkill::OnHit);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;	// 날아가는 방향으로 회전 True
	ProjectileMovement->bShouldBounce = true;	// 벽에 닿으면 튕김
	
	ExplodeRadius = 500.0f;
	DamageAmount = 50.0f;
	KnockBackAmount = 500.0f;
}

void APlayerSkill::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerSkill::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// 플레이어 아니면 Explode
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != GetOwner()))
	{
		Explode();
	}
}

void APlayerSkill::Explode()
{
	// Explode 사운드 재생
	if (ExplodeSound)
	{
		UGameplayStatics:: PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}
	
	// Explode 파티클 재생
	if (ExplodeParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplodeParticle, GetActorLocation(), GetActorRotation(), true);
	}
	
	// 범위 내 엑터 감지
	TArray<AActor*> OverlapActors;	// 효과 적용할 엑터
	TArray<AActor*> IgnoreActors;	// 효과 무시할 엑터
	IgnoreActors.Add(this);		
	IgnoreActors.Add(GetOwner());	// 플레이어는 Skill 효과에서 제외
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	
	// 범위 내 액터 찾기
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		ExplodeRadius,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OverlapActors
	);
	
	// 감지된 액터 처리
	for (AActor* HitActor : OverlapActors)
	{
		if (HitActor && HitActor->ActorHasTag(FName("Enemy")))
		{
			// 데미지 적용
			UGameplayStatics::ApplyDamage(
				HitActor,
				DamageAmount,
				GetInstigatorController(),
				this,
				UDamageType::StaticClass()
			);
			
			// 넉백 적용
			ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
			if (HitCharacter)
			{
				// 방향 계산
				FVector KnockbackDir = HitCharacter->GetActorLocation() - GetActorLocation();
				KnockbackDir.Normalize();
				
				// Z축 보정
				FVector KnockbackVelocity = (KnockbackDir * KnockBackAmount) + FVector(0.0f, 0.0f, KnockBackAmount * 0.5f);
				
				HitCharacter->LaunchCharacter(KnockbackVelocity, true, true);
			}
		}
	}
	
	Destroy();	// 폭발 후 액터 파괴
}



#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Rifle.generated.h"

UCLASS()
class THESEVENTHBULLET_API ARifle : public AWeaponBase
{
	GENERATED_BODY()

public:
	ARifle();
	
	virtual void StartFire() override;
	virtual void Reload() override;
	
	virtual void Fire() override;
	
protected:
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bDrawFireDebug = true; // 발사 디버그 표시 여부
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FireDebugDuration = 1.0f; // 발사 디버그 지속 시간
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Revolver")
	float Damage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Revolver")
	float Range = 3000.f;
};

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Revolver.generated.h"

UCLASS()
class THESEVENTHBULLET_API ARevolver : public AWeaponBase
{
	GENERATED_BODY()

public:
	ARevolver();
	
	virtual void StartFire() override;
	virtual void StopFire() override;
	
	virtual void Fire() override;
	virtual void Reload() override;
	
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

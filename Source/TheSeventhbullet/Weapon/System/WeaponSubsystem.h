#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TheSeventhbullet/Weapon/Data/WeaponDataAsset.h"
#include "TheSeventhbullet/Weapon/Data/WeaponState.h"
#include "WeaponSubsystem.generated.h"

UCLASS()
class THESEVENTHBULLET_API UWeaponSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// 해금여부 판단
	bool IsUnlock(EWeaponTypes WeaponType) const;
	// 해금 로직
	void UnlockWeapon(EWeaponTypes WeaponType);
	// 무기 선택
	void SelectWeapon(EWeaponTypes WeaponType);
	
	// 현재 선택된 Weapon 반환.
	EWeaponTypes GetSelectedWeapon() const;
	// 지정한 무기의 상태를 반환.
	FWeaponState* GetWeaponState(EWeaponTypes WeaponType);
	
private:
	UPROPERTY(VisibleAnywhere)
	TMap<EWeaponTypes, FWeaponState> WeaponStates;
	UPROPERTY(VisibleAnywhere)
	EWeaponTypes SelectedWeapon;
};

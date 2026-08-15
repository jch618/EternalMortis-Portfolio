// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisWeaponBase.h"
#include "Types/MortisStructTypes.h"
#include "MortisShieldBase.generated.h"

DECLARE_DELEGATE_OneParam(FOnShieldBlockDelegate, AActor*)
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisShieldBase : public AMortisWeaponBase
{
	GENERATED_BODY()
	
public:
	FOnShieldBlockDelegate OnShieldBeginBlock;
	FOnShieldBlockDelegate OnShieldEndBlock;

	FORCEINLINE virtual const FMortisWeaponCommonData* GetCommonData() const override { return &EnemyWeaponData.CommonData; }
	
protected:
	//~Begin AActor Interfaces
	virtual void BeginPlay() override;
	//~End AActor Interfaces
	
	//~Begin AMortisCombatItemBase Interfaces
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	//~End AMortisCombatItemBase Interfaces
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Shield");
	FMortisEnemyWeaponData EnemyWeaponData;
};

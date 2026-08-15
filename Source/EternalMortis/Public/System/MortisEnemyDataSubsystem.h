// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MortisEnemyDataSubsystem.generated.h"

class UMortisEnemyScalingData;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyDataSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	const UMortisEnemyScalingData* GetScalingData() const { return CachedScalingData; }

private:
	void LoadEnemyData();

	UPROPERTY()
	TObjectPtr<UMortisEnemyScalingData> CachedScalingData;
};

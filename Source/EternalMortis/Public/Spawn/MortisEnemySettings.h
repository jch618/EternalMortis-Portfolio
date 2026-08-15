// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MortisEnemySettings.generated.h"

class UMortisEnemyScalingData;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Enemy Settings"))
class ETERNALMORTIS_API UMortisEnemySettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Spawning")
	TArray<FDataTableRowHandle> FloorSpawnPools;
	
	UPROPERTY(Config, EditAnywhere, Category = "Scailing")
	TSoftObjectPtr<UMortisEnemyScalingData> EnemyScalingData;
};

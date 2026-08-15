// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisEnemyDataSubsystem.h"

#include "MortisDebugHelper.h"
#include "Spawn/MortisEnemySettings.h"
#include "Character/Enemy/MortisEnemyData.h"

void UMortisEnemyDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	LoadEnemyData();
}

void UMortisEnemyDataSubsystem::LoadEnemyData()
{
	const UMortisEnemySettings* EnemySettings = GetDefault<UMortisEnemySettings>();
	if (!EnemySettings || EnemySettings->EnemyScalingData.IsNull())
	{
		return;
	}	
	CachedScalingData = EnemySettings->EnemyScalingData.LoadSynchronous();
}

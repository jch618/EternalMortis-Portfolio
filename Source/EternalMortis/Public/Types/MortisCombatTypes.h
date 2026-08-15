// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MortisCombatTypes.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisCombatVFXData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TMap<FGameplayTag, FGameplayTag> MaterialToHitCueMap;
};

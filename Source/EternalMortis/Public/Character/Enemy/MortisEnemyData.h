// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Types/MortisEnumTypes.h"
#include "Types/MortisStructTypes.h"
#include "MortisGameplayTags.h"
#include "MortisEnemyData.generated.h"

class UNiagaraSystem;
class UMortisEnemyAbilitySet;
class AMortisEnemyCharacter;
class UBlackboardData;
class UBehaviorTree;
class UMortisAbilitySetBase;
/**
 * 
 */

UCLASS()
class ETERNALMORTIS_API UMortisAttackPatternData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FMortisAttackPattern> AttackPatterns;
};
 
UCLASS()
class ETERNALMORTIS_API UMortisEnemyData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/* Mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<USkeletalMesh> EnemyMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FVector MeshScale = FVector::OneVector;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FVector MeshOffset = FVector::ZeroVector;
	
	/* Character Material Type */
	UPROPERTY(EditDefaultsOnly, Category = "Skin", meta = (Categories = "Character.Material"))
	FGameplayTag SkinMaterialTag = MortisGameplayTags::Character_Material_Flesh;
	
	/* Variation */
	UPROPERTY(EditDefaultsOnly, Category = "Mesh|Variation")
	bool bEnableVisualVariations = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mesh|Variation", meta = (EditCondition = "bEnableVisualVariations"))
	TArray<TObjectPtr<USkeletalMesh>> MeshPool;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh|Variation", meta = (EditCondition = "bEnableVisualVariations"))
	TArray<FMortisMaterialSet> MaterialSetPool;
	
	/* Linked Anim Layer */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> LinkedAnimLayerClass;
	
	/* Capsule Component */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	float CapsuleRadius = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	float CapsuleHalfHeight = 90.f;

	/* Animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float SpeedInterpSpeed = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float DirectionInterpSpeed = 10.f;
	
	/* Combat */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FMortisWeaponCommonData UnarmedData;

	/* UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Boss")
	bool bIsBoss = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Boss", meta = (EditCondition = "bIsBoss"))
	FText BossDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Boss", meta = (EditCondition = "bIsBoss"))
	FText BossClearText;
	
	/* Movement */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FRotator RotationRate = FRotator(0.f, 180.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float IdleMaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float StrafingMaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ChasingMaxWalkSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BrakingDecelerationWalking = 1000.f;
	
	/* Ability system */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TObjectPtr<UMortisEnemyAbilitySet> AbilitySet;

	/* AI */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	/* EQS */
	UPROPERTY(EditDefaultsOnly, Category = "AI|EQS")
	TMap<FGameplayTag, FMortisDistanceRange> PhaseStrafingRanges; 
	
	/* Perception */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float SightRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float LoseSightRadius = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float PeripheralVisionAngleDegrees = 210.f;

	/* Detour Crowd Avoidance */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config")
	bool bEnableCrowdAvoidance = true;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance", UIMIN = "1", UIMAX = "4"))
	int32 AvoidanceQuality = 4; 

	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance"))
	float AvoidanceQueryRange = 600.f;

	/* Team Agent */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	EMortisTeam Affiliation = EMortisTeam::Enemy;

	/* Attack Pattern */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UMortisAttackPatternData> AttackPatternData;
	
	/* Stat */
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	FDataTableRowHandle StatRowHandle;
};

USTRUCT(BlueprintType)
struct ETERNALMORTIS_API FMortisEnemyStats : public FTableRowBase
{
	GENERATED_BODY()
	
	/* Default Stat */
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float MaxHealth = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float BaseDamage = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float MaxPoise = 20.f;
	
	/* Resist */
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float ResistPierce = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float ResistBlunt = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float ResistSlash = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float ResistMagic = 0.f;

	/* Drop */
	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	FMortisEnemyDropData DropData;
};

namespace MortisScalingCurveNames
{
	const FName MaxHealth = TEXT("MaxHealth");
	const FName BaseDamage = TEXT("BaseDamage");
	const FName MaxPoise = TEXT("MaxPoise");
}

UCLASS()
class ETERNALMORTIS_API UMortisEnemyScalingData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Scaling")
	TObjectPtr<UCurveTable> ScalingCurveTable;

	float GetMultiplier(FName StatsName, int32 Floor) const;
};

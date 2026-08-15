// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "MortisGameplayTags.h"

#include "GameplayTagContainer.h"
#include "MortisEnumTypes.h"
#include "MortisStructTypes.generated.h"

class UAbilitySystemComponent;
class UMortisSpawnConfig;
class UGameplayEffect;
class UMortisPlayerLinkedAnimLayer;
class UInputMappingContext;
class UMortisPlayerGameplayAbility;
class UAnimMontage;
enum class EMortisStatGrade : uint8;
enum class EMortisShopTransactionType : uint8;

USTRUCT(BlueprintType)
struct FMortisWeaponCommonData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WeaponDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PoiseDamage = 0.f;
	
};

USTRUCT(BlueprintType)
struct FMortisEnemyWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMortisWeaponCommonData CommonData;
};

USTRUCT(BlueprintType)
struct FMortisAttackPatternStep
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackStyleTag = MortisGameplayTags::Data_AttackStyle_Swing;
	
	UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier = 1.0f;

	/* Motion Warping */
	UPROPERTY(EditDefaultsOnly)
	bool bUseMotionWarping = true;

	UPROPERTY(EditDefaultsOnly)
	FName WarpTargetName = TEXT("AttackTarget");

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseMotionWarping"))
	EMortisWarpTargetMode WarpTargetMode = EMortisWarpTargetMode::ActorLocation;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseMotionWarping"))
	float DesiredDistance = 100.f;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseMotionWarping"))
	bool bContinuousWarpUpdate = true;

	/* Gameplay Effect */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> OverrideDamageEffectClass;
	
	/* Spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMortisSpawnConfig> SpawnConfigClass;
	
	/* Combo */
	UPROPERTY(EditDefaultsOnly)
	bool bUseComboTransitionNotify = false;
};

USTRUCT(BlueprintType)
struct FMortisAttackPattern
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FMortisAttackPatternStep> Steps;

	UPROPERTY(EditDefaultsOnly)
	float MinRange = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxRange = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float MinAngle = -120.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxAngle = 120.f;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredPhases;
	
	UPROPERTY(EditDefaultsOnly)
	float Weight = 1.0f;
};

USTRUCT(BlueprintType)
struct FMortisTagToBoolMapping
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mappping")
	FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mappping")
	FName BoolPropertyName;
};

// for EQS
USTRUCT(BlueprintType)
struct FMortisDistanceRange
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Distance")
	float MinDistance = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Distance")
	float MaxDistance = 500.f;
};

USTRUCT(BlueprintType)
struct FMortisAttackTraceConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float Radius = 5.f;
	
	UPROPERTY(EditDefaultsOnly)
	FName StartSocket;

	UPROPERTY(EditDefaultsOnly)
	FName EndSocket;

	UPROPERTY(EditDefaultsOnly)
	EMortisMeshSource MeshSource = EMortisMeshSource::WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MeshSource == EMortisMeshSource::WeaponMesh", Categories = "Data.Weapon.Slot"))
	FGameplayTag SlotTag = MortisGameplayTags::Data_Weapon_Slot_MainHand;
	
	bool operator==(const FMortisAttackTraceConfig& Config) const
	{
		return StartSocket == Config.StartSocket && EndSocket == Config.EndSocket && MeshSource == Config.MeshSource;
	}
};

USTRUCT(BlueprintType)
struct FMortisMaterialSet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	TArray<TObjectPtr<UMaterialInterface>> Materials;
};

USTRUCT(BlueprintType)
struct FMortisEnemyDropData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	int32 MinGold = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	int32 MaxGold = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	int32 MinMemoryFragmentCount = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	int32 MaxMemoryFragmentCount = 100.f;
};

USTRUCT(BlueprintType)
struct FMortisFloorMusicData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> ExplorationBGM;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> BossBGM;
};
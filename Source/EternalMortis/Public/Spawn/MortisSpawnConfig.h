// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/MortisStructTypes.h"
#include "MortisSpawnConfig.generated.h"

class AMortisHitboxBase;
class AMortisEnemyCharacter;
class AMortisSpawnIndicator;
class AMortisCharacterBase;
class AMortisProjectileBase;
struct FGameplayEffectSpecHandle;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ETERNALMORTIS_API UMortisSpawnConfig : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle);
	
protected:
	void SpawnWithIndicatorOrImmediate(UWorld* World, TSubclassOf<AActor> ClassToSpawn, const FVector& Location, const FRotator& Rotation, const
		FGameplayEffectSpecHandle& DamageSpecHandle, APawn* Instigator);
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	EMortisSpawnLocationType SpawnLocationType = EMortisSpawnLocationType::InstigatorSocket;
	
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (EditCondition = "SpawnLocationType == EMortisSpawnLocationType::InstigatorSocket || SpawnLocationType == EMortisSpawnLocationType::InstigatorWeaponSocket"))
	FName SpawnSocketName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (EditCondition = "SpawnLocationType == EMortisSpawnLocationType::InstigatorWeaponSocket", Categories = "Data.Weapon.Slot"))
	FGameplayTag WeaponSlotTag;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector RelativeOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMortisSpawnIndicator> IndicatorClass;

	FTransform GetSpawnTransform(const AMortisCharacterBase* Instigator, const AActor* Target) const;
};


UCLASS(DisplayName = "Single Hitbox")
class ETERNALMORTIS_API UMortisSpawnConfig_SingleHitbox : public UMortisSpawnConfig
{
	GENERATED_BODY()
	
public:
	virtual void Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Hitbox")
	TSubclassOf<AMortisHitboxBase> HitboxClass;
};

UCLASS(DisplayName = "Single Projectile")
class ETERNALMORTIS_API UMortisSpawnConfig_SingleProjectile : public UMortisSpawnConfig
{
	GENERATED_BODY()
	
public:
	virtual void Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AMortisProjectileBase> ProjectileClass;
};

// sector projectile
UCLASS(DisplayName = "Multi Projectile")
class ETERNALMORTIS_API UMortisSpawnConfig_MultiProjectile : public UMortisSpawnConfig
{
	GENERATED_BODY()

public:
	virtual void Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AMortisProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (ClampMin = "1"))
	int32 ProjectileCount = 3;

	// 전체 부채꼴 각도 (ex. 60이면 각 투사체가 -30 ~ +30 범위에 배치)
	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (ClampMin = "0.0"))
	float SpreadAngleDegrees = 60.f;
};


UCLASS(DisplayName = "Ground Effect Spawn")
class ETERNALMORTIS_API UMortisSpawnConfig_GroundSpawn : public UMortisSpawnConfig
{
	GENERATED_BODY()

public:
	virtual void Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> EffectActorClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<FVector> RelativeGroundEffectLocations;
};


UCLASS(DisplayName = "Summon Enemy")
class ETERNALMORTIS_API UMortisSpawnConfig_SummonEnemy : public UMortisSpawnConfig
{
	GENERATED_BODY()

public:
	virtual void Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Summon")
	FDataTableRowHandle CustomEnemySpawnRowHandle;
	
	UPROPERTY(EditAnywhere, Category = "Summon")
	float SpawnRadius = 300.f;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EMortisAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UENUM()
enum class EMortisValidType : uint8
{
	Valid,
	InValid
};

UENUM()
enum class EMortisSuccessType : uint8
{
	Success,
	Failed
};

UENUM()
enum class EMortisTeam : uint8
{
	Player = 0 UMETA(DisplayName = "Player"),
	Enemy = 1 UMETA(DisplayName = "Enemy"),
	Neutral = 2 UMETA(DisplayName = "Neutral"),
};

UENUM(BlueprintType)
enum class EMortisWarpTargetMode : uint8
{
	ActorLocation UMETA(DisplayName = "Actor Location"),
	OffsetFromSelf UMETA(DisplayName = "Offset From Self"),
	OffsetFromTarget UMETA(DisplayName = "Offset From Target")
};

UENUM()
enum class EMortisMeshSource : uint8
{
	WeaponMesh UMETA(DisplayName = "Weapon Mesh"),
	CharacterMesh UMETA(DisplayName = "Character Mesh"),
};

UENUM(BlueprintType)
enum class EMortisSpawnLocationType : uint8
{
	InstigatorSocket UMETA(DisplayName = "Instigator Socket"),
	TargetLocation UMETA(DisplayName = "Target Location"),
	InstigatorLocation UMETA(DisplayName = "Instigator Location"),
	InstigatorWeaponSocket UMETA(DisplayName = "Instigator Weapon Socket"),
};
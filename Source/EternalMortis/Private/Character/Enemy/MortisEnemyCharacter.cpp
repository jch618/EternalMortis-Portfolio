// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MortisEnemyCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MortisDebugHelper.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Data/MortisEnemyAbilitySet.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"
#include "AbilitySystem/Attributes/MortisEnemyAttributeSet.h"
#include "Animation/MortisEnemyAnimInstance.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Components/CapsuleComponent.h"
#include "Controllers/MortisAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Spawn/MortisEnemySettings.h"
#include "System/MortisEnemyDataSubsystem.h"
#include "System/MortisGameInstance.h"
#include "System/MortisMetaProgressionSubsystem.h"
#include "System/MortisRunStateSubsystem.h"
#include "UObject/ConstructorHelpers.h"

AMortisEnemyCharacter::AMortisEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMortisEnemyAttributeSet>(TEXT("MortisAttributeSet")))
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	EnemyCombatComponent = CreateDefaultSubobject<UMortisEnemyCombatComponent>("EnemyCombatComponent");

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMortisEnemyCharacter::InitializeEnemyCharacter()
{
	InitializeEnemyByData();
	RegisterStateTagEvent();
}

void AMortisEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::PrintRotation, 0.01f, true, -1);
}

void AMortisEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMortisEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// PrintRotation();
	// bool A = GetCharacterMovement()->bOrientRotationToMovement;
	// bool B = GetCharacterMovement()->bUseControllerDesiredRotation;
	// bool C = bUseControllerRotationYaw;
	// FString DebugString = FString::Printf(TEXT("OrientToMovement: %s, UseControllerRotation: %s, UseControllerYaw: %s"), *Debug::ToString(A), *Debug::ToString(B), *Debug::ToString(C));
	// DrawDebugString(GetWorld(), GetActorLocation() + FVector(0.f, -50.f, 100.f), DebugString, this, FColor::Yellow, 0.f);
	//
}

void AMortisEnemyCharacter::StartDeath()
{
	Super::StartDeath();
	
	if (AMortisAIController* AIC = Cast<AMortisAIController>(GetController()))
	{
		AIC->OnEnemyDead();
	}
	// GetMesh()->SetSimulatePhysics(true);
}

void AMortisEnemyCharacter::FinishDeath()
{
	// Drop Items
	if (GetWorld() && GetWorld()->GetGameInstance() && EnemyData)
	{
		if (UMortisRunStateSubsystem* RunStateSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMortisRunStateSubsystem>())
		{
			RunStateSubsystem->AddGold(FMath::RandRange(EnemyStats.DropData.MinGold, EnemyStats.DropData.MaxGold));
			// MORTIS_LOG("Gold: %d", RunStateSubsystem->GetCurrentGold());
		}
		
		if (UMortisMetaProgressionSubsystem* MetaProgressionSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMortisMetaProgressionSubsystem>())
		{
			MetaProgressionSubsystem->AddMemoryFragments(FMath::RandRange(EnemyStats.DropData.MinGold, EnemyStats.DropData.MaxGold));
			// MORTIS_LOG("Memory Fragments: %d", MetaProgressionSubsystem->GetMemoryFragments());
		}
	}
	
	Super::FinishDeath();
}

void AMortisEnemyCharacter::InitializeEnemyByData()
{
	if (!ensureMsgf(EnemyData, TEXT("%s: Enemy Data is not set"), *GetName()))
	{
		return;
	}

	check(GetCapsuleComponent() && GetMesh() && GetCharacterMovement());
	
	GetCapsuleComponent()->InitCapsuleSize(EnemyData->CapsuleRadius, EnemyData->CapsuleHalfHeight);
	
	USkeletalMesh* SelectedMesh = EnemyData->EnemyMesh;
	if (EnemyData->bEnableVisualVariations && !EnemyData->MeshPool.IsEmpty())
	{
		int32 RandomIndex = FMath::RandRange(0, EnemyData->MeshPool.Num() - 1);
		if (EnemyData->MeshPool[RandomIndex])
		{
			SelectedMesh = EnemyData->MeshPool[RandomIndex];
		}
	}

	GetMesh()->SetSkeletalMesh(SelectedMesh);
	
	if (MortisAbilitySystemComponent)
	{
		MortisAbilitySystemComponent->AddLooseGameplayTag(EnemyData->SkinMaterialTag);
	}
	
	if (EnemyData->bEnableVisualVariations && !EnemyData->MaterialSetPool.IsEmpty())
	{
		int32 RandomIndex = FMath::RandRange(0, EnemyData->MaterialSetPool.Num() - 1);
		ApplyMaterialSet(EnemyData->MaterialSetPool[RandomIndex]);
	}
	
	GetMesh()->SetRelativeScale3D(EnemyData->MeshScale);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -EnemyData->CapsuleHalfHeight) + EnemyData->MeshOffset);
	GetMesh()->SetRelativeRotation(EnemyData->MeshRotation);

	if (EnemyData->AnimClass)
	{
		GetMesh()->SetAnimInstanceClass(EnemyData->AnimClass);
	}

	if (EnemyData->LinkedAnimLayerClass)
	{
		GetMesh()->LinkAnimClassLayers(EnemyData->LinkedAnimLayerClass);
	}
	
	GetCharacterMovement()->RotationRate = EnemyData->RotationRate;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData->IdleMaxWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = EnemyData->BrakingDecelerationWalking;

	CharacterAbilitySet = EnemyData->AbilitySet;
	
	// is Game playing
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		if (EnemyData->AttackPatternData)
		{
			if (EnemyCombatComponent)
			{
				EnemyCombatComponent->SetAttackPattern(EnemyData->AttackPatternData);
			}
		}
	}
}

UMortisEnemyData* AMortisEnemyCharacter::GetEnemyData() const
{
	return EnemyData;
}

UMortisCombatComponent* AMortisEnemyCharacter::GetCombatComponent() const
{
	return EnemyCombatComponent;
}

void AMortisEnemyCharacter::OnSpawned(const FGameplayEffectSpecHandle& EffectSpecHandle)
{
	InitializeEnemyCharacter();
	if (AMortisEnemyCharacter* Enemy = Cast<AMortisEnemyCharacter>(GetInstigator()))
	{
		Enemy->OnStartDeath.AddUObject(this, &ThisClass::OnOwnerEnemyDestroyed);
		Enemy->OnDestroyed.AddDynamic(this, &ThisClass::OnOwnerEnemyDestroyed);
	}
}

UMortisEnemyCombatComponent* AMortisEnemyCharacter::GetEnemyCombatComponent() const
{
	return EnemyCombatComponent;
}

float AMortisEnemyCharacter::GetRandomStrafingDistance() const
{
	if (!EnemyData || !EnemyData->PhaseStrafingRanges.Contains(CurrentPhase))
	{
		return 500.f;
	}
	return FMath::RandRange(EnemyData->PhaseStrafingRanges[CurrentPhase].MinDistance, EnemyData->PhaseStrafingRanges[CurrentPhase].MaxDistance);
}

void AMortisEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	ApplyEnemyStats();
}

void AMortisEnemyCharacter::ApplyMaterialSet(const FMortisMaterialSet& MaterialSet)
{
	for (int32 i = 0; i < MaterialSet.Materials.Num(); i++)
	{
		if (UMaterialInterface* Material = MaterialSet.Materials[i])
		{
			GetMesh()->SetMaterial(i, Material);
		}
	}
}

#if WITH_EDITOR
void AMortisEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.GetPropertyName();
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, EnemyData))
	{
		if (EnemyData)
		{
			InitializeEnemyByData();
		}
	}
}
#endif

void AMortisEnemyCharacter::ApplyEnemyStats()
{
	if (!EnemyData)
	{
		MORTIS_LOG("Enemy Data is null");
		return;
	}
	
	FMortisEnemyStats* EnemyStatPointer = EnemyData->StatRowHandle.GetRow<FMortisEnemyStats>(TEXT("Enemy Stats"));
	if (!EnemyStatPointer)
	{
		MORTIS_LOG("Fail to get Stats");
		return;
	}
	
	EnemyStats = *EnemyStatPointer;
	ApplyFloorScaling(EnemyStats);
	
	if (!EnemyData->AbilitySet)
	{
		return;
	}
	EnemyData->AbilitySet->GiveToEnemy(MortisAbilitySystemComponent, EnemyStats);
}

void AMortisEnemyCharacter::ApplyFloorScaling(FMortisEnemyStats& EnemyStatsToApply) const
{	
	// MORTIS_LOG("");
	if (!GetWorld() || !GetWorld()->GetGameInstance())
	{
		return;
	}
	
	if (EnemyData->bIsBoss)
	{
		return;
	}
	const UMortisRunStateSubsystem* RunStateSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMortisRunStateSubsystem>();
	const UMortisEnemyDataSubsystem* EnemyDataSubsystem = GetWorld()->GetSubsystem<UMortisEnemyDataSubsystem>();
	if (!RunStateSubsystem || !EnemyDataSubsystem)
	{
		MORTIS_LOG("Fail to get Subsystem");
		return;
	}
	
	const UMortisEnemyScalingData* ScalingData = EnemyDataSubsystem->GetScalingData();
	if (!ScalingData)
	{
		MORTIS_LOG("Fail to get ScalingData");
		return;
	}
	
	float CurrentFloor = RunStateSubsystem->GetCurrentFloor();
	EnemyStatsToApply.BaseDamage *= ScalingData->GetMultiplier(MortisScalingCurveNames::BaseDamage, CurrentFloor);
	EnemyStatsToApply.MaxHealth *= ScalingData->GetMultiplier(MortisScalingCurveNames::MaxHealth, CurrentFloor);
	EnemyStatsToApply.MaxPoise *= ScalingData->GetMultiplier(MortisScalingCurveNames::MaxPoise, CurrentFloor);
}

void AMortisEnemyCharacter::RegisterStateTagEvent()
{
	if (!MortisAbilitySystemComponent)
	{
		MORTIS_LOG("ASC is null");
		return;
	}
	
	MortisAbilitySystemComponent->RegisterGameplayTagEvent(
		MortisGameplayTags::State_Movement_Strafing,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &ThisClass::OnStrafingStateChanged);

	MortisAbilitySystemComponent->RegisterGameplayTagEvent(
		MortisGameplayTags::State_Movement_Chasing,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &ThisClass::OnChasingStateChanged);
}

void AMortisEnemyCharacter::OnStrafingStateChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (!EnemyData)
	{
		return;
	}
	bool bStrafing = NewCount > 0;
	// MORTIS_LOG("Strafing %s", NewCount > 0 ? TEXT("Begin") : TEXT("End"));
	// bUseControllerRotationYaw = bStrafing;
	GetCharacterMovement()->bOrientRotationToMovement = !bStrafing;
	GetCharacterMovement()->bUseControllerDesiredRotation = bStrafing;
	GetCharacterMovement()->MaxWalkSpeed = bStrafing ? EnemyData->StrafingMaxWalkSpeed : EnemyData->IdleMaxWalkSpeed;
}

void AMortisEnemyCharacter::OnChasingStateChanged(FGameplayTag Tag, int32 NewCount) const
{
	if (!EnemyData)
	{
		return;
	}
	// MORTIS_LOG("Chasing %s", NewCount > 0 ? TEXT("Begin") : TEXT("End"));
	bool bChasing = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bChasing ? EnemyData->ChasingMaxWalkSpeed : EnemyData->IdleMaxWalkSpeed;
}

void AMortisEnemyCharacter::OnOwnerEnemyDestroyed(AActor* OwnerEnemy)
{
	FGameplayEventData Data;
	Data.Instigator = OwnerEnemy;
	Data.EventTag = MortisGameplayTags::Event_Dead;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Data.EventTag, Data);
}

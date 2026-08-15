// Fill out your copyright notice in the Description page of Project Settings.


#include "MortisFunctionLibrary.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Controllers/MortisAIController.h"
#include "Interfaces/MortisCombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "Components/Combat/MortisCombatComponent.h"

bool UMortisFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);

	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}

	return false;
}

UMortisCombatComponent* UMortisFunctionLibrary::GetCombatComponent(const AActor* Actor)
{
	if (const IMortisCombatInterface* CombatInterface = Cast<IMortisCombatInterface>(Actor))
	{
		return CombatInterface->GetCombatComponent();
	}
	return nullptr;
}

UMortisCombatComponent* UMortisFunctionLibrary::BP_GetCombatComponent(const AActor* Actor, EMortisValidType& OutValidType)
{
	UMortisCombatComponent* CombatComponent = GetCombatComponent(Actor); 
	OutValidType = CombatComponent ? EMortisValidType::Valid : EMortisValidType::InValid;
	return CombatComponent;
}

bool UMortisFunctionLibrary::HasGameplayTag(AActor* Actor, FGameplayTag TagToCheck)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}

void UMortisFunctionLibrary::AddGameplayTag(AActor* Actor, const FGameplayTag& Tag)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	if (ASC && !ASC->HasMatchingGameplayTag(Tag))
	{
		ASC->AddLooseGameplayTag(Tag);
	}
}

void UMortisFunctionLibrary::RemoveGameplayTag(AActor* Actor, const FGameplayTag& Tag)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	if (ASC && ASC->HasMatchingGameplayTag(Tag))
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}

AMortisEnemyCharacter* UMortisFunctionLibrary::GetEnemyCharacter(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	return AIC ? Cast<AMortisEnemyCharacter>(AIC->GetPawn()) : nullptr;
}

AMortisAIController* UMortisFunctionLibrary::GetMortisAIController(UBehaviorTreeComponent& OwnerComp)
{
	return Cast<AMortisAIController>(OwnerComp.GetAIOwner());
}

UMortisAbilitySystemComponent* UMortisFunctionLibrary::GetMortisASC(UBehaviorTreeComponent& OwnerComp)
{
	AMortisEnemyCharacter* Enemy = GetEnemyCharacter(OwnerComp);
	return Enemy ? Enemy->GetMortisAbilitySystemComponent() : nullptr;
}

FVector UMortisFunctionLibrary::CalculateWarpTargetLocation(const AActor* SelfActor, const AActor* TargetActor, EMortisWarpTargetMode WarpTargetMode, float DesiredDistance)
{
	if (!SelfActor || !TargetActor)
	{
		return FVector::ZeroVector;
	}
	const FVector TargetLocation = TargetActor->GetActorLocation();
	const FVector SelfLocation = SelfActor->GetActorLocation();
	switch (WarpTargetMode)
	{
	case EMortisWarpTargetMode::ActorLocation:
		return TargetLocation;
		
	case EMortisWarpTargetMode::OffsetFromSelf:
		{
			FVector Direction = TargetLocation - SelfLocation;
			return SelfLocation + Direction.GetSafeNormal2D() * DesiredDistance;			
		}

	case EMortisWarpTargetMode::OffsetFromTarget:
		{
			FVector Direction = SelfLocation - TargetLocation;
			return TargetLocation + Direction.GetSafeNormal2D() * DesiredDistance;
		}
		
	default:
		return TargetLocation;
	}
}

AMortisWeaponBase* UMortisFunctionLibrary::GetWeaponFromCurrentSlot(const UMeshComponent* MeshComp)
{
	if (!MeshComp)
	{
		return nullptr;
	}
	IMortisCombatInterface* CombatInterface = Cast<IMortisCombatInterface>(MeshComp->GetOwner());
	if (!CombatInterface)
	{
		return nullptr;
	}

	UMortisCombatComponent* CombatComponent = CombatInterface->GetCombatComponent();
	if (!CombatComponent)
	{
		return nullptr;
	}
	return CombatComponent->GetCurrentWeapon();
}

AMortisWeaponBase* UMortisFunctionLibrary::GetWeaponFromSlot(const AActor* Owner, const FGameplayTag& SlotTag)
{
	const IMortisCombatInterface* CombatInterface = Cast<IMortisCombatInterface>(Owner);
	if (!CombatInterface)
	{
		return nullptr;
	}
	
	UMortisCombatComponent* CombatComponent = CombatInterface->GetCombatComponent();
	if (!CombatComponent)
	{
		return nullptr;
	}
	
	return CombatComponent->GetEquippedWeaponBySlotTag(SlotTag);
}

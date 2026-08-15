// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisBossSubsystem.h"

#include "MortisDebugHelper.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Components/Combat/MortisCombatComponent.h"

void UMortisBossSubsystem::NotifyBossEncountered(AMortisEnemyCharacter* BossEnemy)
{
	if (!BossEnemy)
	{
		return;
	}
	// MORTIS_LOG("Notify boss encountered");
	
	CurrentBoss = BossEnemy;
	OnBossEncountered.Broadcast(BossEnemy);
}

void UMortisBossSubsystem::NotifyBossDefeated()
{
	// MORTIS_LOG("Notify boss defeated");
	
	bHasBossDefeated = true;
	CurrentBoss.Reset();
	OnBossDefeated.Broadcast();
}

void UMortisBossSubsystem::ResetBossForRevive()
{
	if (!CurrentBoss.IsValid())
	{
		return;
	}
	
	if (CurrentBoss->GetCombatComponent())
	{
		CurrentBoss->GetCombatComponent()->ClearWeapons();
	}
	// MORTIS_LOG("Reset Boss For Revive");
	CurrentBoss->Destroy();
	CurrentBoss.Reset();
	OnBossReset.Broadcast();
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MortisBossSubsystem.generated.h"

class AMortisEnemyCharacter;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossEncountered, AMortisEnemyCharacter*, BossEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDefeated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossReset);
UCLASS()
class ETERNALMORTIS_API UMortisBossSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Mortis|Boss")
	FOnBossEncountered OnBossEncountered;

	UPROPERTY(BlueprintAssignable, Category = "Mortis|Boss")
	FOnBossDefeated OnBossDefeated;
	
	UPROPERTY(BlueprintAssignable, Category = "Mortis|Boss")
	FOnBossReset OnBossReset;
	
	UFUNCTION(BlueprintCallable, Category = "Mortis|Boss")
	void NotifyBossEncountered(AMortisEnemyCharacter* BossEnemy);
	
	UFUNCTION(BlueprintCallable, Category = "Mortis|Boss")
	void NotifyBossDefeated();

	UFUNCTION(BlueprintCallable, Category = "Mortis|Boss")
	FORCEINLINE AMortisEnemyCharacter* GetCurrentBoss() const { return CurrentBoss.Get(); }
	
	UFUNCTION(BlueprintCallable, Category = "Mortis|Boss")
	FORCEINLINE bool IsBossActive() const { return CurrentBoss.IsValid(); }
	
	UFUNCTION(BlueprintCallable, Category = "Mortis|Boss")
	void ResetBossForRevive();
	
	UFUNCTION(BlueprintCallable, Category = "Mortis|Boss")
	FORCEINLINE bool HasBossDefeated() const { return bHasBossDefeated; }
private:
	TWeakObjectPtr<AMortisEnemyCharacter> CurrentBoss;
	bool bHasBossDefeated = false;
};

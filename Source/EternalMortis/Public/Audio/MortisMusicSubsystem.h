// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MortisMusicSubsystem.generated.h"

class AMortisEnemyCharacter;

UCLASS()
class ETERNALMORTIS_API UMortisMusicSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:	
	//~ Begin UWorldSubsystem Interfaces
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	//~ End UWorldSubsystem Interfaces

	UFUNCTION(BlueprintCallable)
	void InitializeMusicSubsystem();
	
protected:
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> ActiveAudioComponent;
	
	UPROPERTY()
	TObjectPtr<USoundBase> ExplorationBGM;
	
	UPROPERTY()
	TObjectPtr<USoundBase> BossBGM;
	
	UPROPERTY()
	float FadeDuration = 1.f;
	
	UFUNCTION()
	void OnBossEncountered(AMortisEnemyCharacter* BossEnemy);
	
	UFUNCTION()
	void OnBossReset();
	
	UFUNCTION()
	void OnBossDefeated();
	
private:
	void ChangeBGM(USoundBase* NewSound);
	
	void LoadCurrentFloorMusic();
};

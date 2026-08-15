// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/MortisMusicSubsystem.h"

#include "MortisDebugHelper.h"
#include "System/MortisBossSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/MortisGameDataSettings.h"
#include "System/MortisRunStateSubsystem.h"
#include "Types/MortisStructTypes.h"

void UMortisMusicSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// MORTIS_LOG("");
	
	// LoadCurrentFloorMusic();
	//
	// ChangeBGM(ExplorationBGM);
	//
	// if (ActiveAudioComponent && ExplorationBGM)
	// {
	// 	// MORTIS_LOG("Set Sound: %s", *ExplorationBGM->GetName());
	// 	ActiveAudioComponent->SetSound(ExplorationBGM);
	// 	ActiveAudioComponent->FadeIn(FadeDuration);
	// }
	//
	// if (UMortisBossSubsystem* BossSubsystem = GetWorld()->GetSubsystem<UMortisBossSubsystem>())
	// {
	// 	BossSubsystem->OnBossEncountered.AddDynamic(this, &UMortisMusicSubsystem::OnBossEncountered);
	// 	BossSubsystem->OnBossReset.AddDynamic(this, &UMortisMusicSubsystem::OnBossReset);
	// 	BossSubsystem->OnBossDefeated.AddDynamic(this, &UMortisMusicSubsystem::OnBossDefeated);
	// }
	InitializeMusicSubsystem();
}

void UMortisMusicSubsystem::InitializeMusicSubsystem()
{
	// MORTIS_LOG("");
	
	LoadCurrentFloorMusic();
	
	ChangeBGM(ExplorationBGM);
	
	if (ActiveAudioComponent && ExplorationBGM)
	{
		// MORTIS_LOG("Set Sound: %s", *ExplorationBGM->GetName());
		ActiveAudioComponent->SetSound(ExplorationBGM);
		ActiveAudioComponent->FadeIn(FadeDuration);
	}
	
	if (UMortisBossSubsystem* BossSubsystem = GetWorld()->GetSubsystem<UMortisBossSubsystem>())
	{
		BossSubsystem->OnBossEncountered.AddDynamic(this, &UMortisMusicSubsystem::OnBossEncountered);
		BossSubsystem->OnBossReset.AddDynamic(this, &UMortisMusicSubsystem::OnBossReset);
		BossSubsystem->OnBossDefeated.AddDynamic(this, &UMortisMusicSubsystem::OnBossDefeated);
	}
}

void UMortisMusicSubsystem::OnBossEncountered(AMortisEnemyCharacter* BossEnemy)
{
	ChangeBGM(BossBGM);
}

void UMortisMusicSubsystem::OnBossReset()
{
	ChangeBGM(ExplorationBGM);
}

void UMortisMusicSubsystem::OnBossDefeated()
{
	ChangeBGM(ExplorationBGM);
}

void UMortisMusicSubsystem::ChangeBGM(USoundBase* NewSound)
{
	if (!NewSound)
	{
		return;
	}
	
	if (ActiveAudioComponent && ActiveAudioComponent->GetSound() == NewSound && ActiveAudioComponent->IsPlaying())
	{
		return;
	}
	
	if (ActiveAudioComponent)
	{
		ActiveAudioComponent->FadeOut(FadeDuration, 0.f);
	}
	
	ActiveAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), NewSound, 1.f, 1.f, 0.f, nullptr, false, true);
	if (ActiveAudioComponent && NewSound)
	{
		// MORTIS_LOG("NewSound: %s", *NewSound->GetName());
		ActiveAudioComponent->FadeIn(FadeDuration);
	}
}

void UMortisMusicSubsystem::LoadCurrentFloorMusic()
{
	// MORTIS_LOG("Load Current Floor Music");
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		MORTIS_LOG("GameInstance is null");
		return;
	}
	
	UMortisRunStateSubsystem* RunStateSubsystem = GameInstance->GetSubsystem<UMortisRunStateSubsystem>();
	if (!RunStateSubsystem)
	{
		MORTIS_LOG("Failed to get run state subsystem");
		return;
	}
	
	int32 CurrentFloor = RunStateSubsystem->GetCurrentFloor();
	
	const UMortisGameDataSettings* GameDataSettings = GetDefault<UMortisGameDataSettings>();
	if (!GameDataSettings || GameDataSettings->FloorMusicDataTable.IsNull())
	{
		MORTIS_LOG("GameSettings or FloorMusicDataTable soft ptr is null");
		return;
	}
	
	UDataTable* FloorMusicDataTable = GameDataSettings->FloorMusicDataTable.LoadSynchronous();
	if (!FloorMusicDataTable)
	{
		MORTIS_LOG("FloorMusicDataTable is null");
		return;
	}
	
	const FString RowNameString = FString::Printf(TEXT("Floor_%d"), CurrentFloor);
	const FName RowName = FName(*RowNameString);
	
	FMortisFloorMusicData* FloorMusicData = FloorMusicDataTable->FindRow<FMortisFloorMusicData>(RowName, TEXT("Floor Music"));
	if (!FloorMusicData)
	{
		MORTIS_LOG("Fail to get row");
		return;
	}
	
	// MORTIS_LOG("Current Floor: %d, Music: %s", RunStateSubsystem->GetCurrentFloor(), *FloorMusicData->ExplorationBGM->GetName());
	
	ExplorationBGM = FloorMusicData->ExplorationBGM;
	BossBGM = FloorMusicData->BossBGM;
}

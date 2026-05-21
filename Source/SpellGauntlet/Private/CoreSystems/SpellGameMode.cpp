// Fill out your copyright notice in the Description page of Project Settings.


#include "Coresystems/SpellGameMode.h"
#include "Coresystems/SpellGameState.h"
#include "Player/SpellPlayerState.h"
#include "Gameplay/AbilitySystem/SpellAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Coresystems/SpellGameplayTags.h"

void ASpellGameMode::PlayerKilled(AController* Killer, AController* Victim)
{
    // Creating quick function for grabbing playerstate with controller
    auto GetPlayerState = [](AController* C) {
        return (C ? C->GetPlayerState<ASpellPlayerState>() : nullptr);
        };

    ASpellPlayerState* VictimSpellPS = GetPlayerState(Victim);
    ASpellPlayerState* KillerSpellPS = GetPlayerState(Killer);

    if (VictimSpellPS)
    {
        VictimSpellPS->DeathCount++;
    }

    // No points for killing self
    if (KillerSpellPS && KillerSpellPS != VictimSpellPS)
    {
        KillerSpellPS->KillCount++;
        UpdatePlayerScore(KillerSpellPS, ScoreOnPlayerKill);
    }

    RequestRespawn(Victim);
}

void ASpellGameMode::UpdatePlayerScore(ASpellPlayerState* SpellPS, int32 ScoreIncrement)
{
    ASpellGameState* GS = GetGameState<ASpellGameState>();
    if (GS)
    {
        SpellPS->UpdateScore(SpellPS->GetScore() + ScoreIncrement);

        CheckWinCondition(SpellPS);
    }
}

void ASpellGameMode::CheckWinCondition(ASpellPlayerState* SpellPS)
{
    if (SpellPS->GetScore() >= ScoreToWin)
    {
        EndMatch();

        FTimerHandle RestartTimerHandle;

        GetWorldTimerManager().SetTimer(RestartTimerHandle, this, &ASpellGameMode::RestartGame, GameRestartTimer, false);
    }
}

void ASpellGameMode::RequestRespawn(AController* Controller)
{
    if (Controller)
    {
        FTimerHandle RespawnTimerHandle;
        FTimerDelegate RespawnDelegate;

        RespawnDelegate.BindUObject(this, &ASpellGameMode::RespawnPlayer, Controller);

        GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
    }
}

AActor* ASpellGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    if (SpawnPoints.IsEmpty())
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPoints);
    }

    // Pick random spawn point from array
    int spawnIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
    AActor* chosenSpawnPoint = SpawnPoints[spawnIndex];

    // Remove it so it cant be picked again
    SpawnPoints.RemoveAt(spawnIndex);

    return chosenSpawnPoint;
}

void ASpellGameMode::RespawnPlayer(AController* Controller)
{
    if (ASpellPlayerState* SpellPS = Controller->GetPlayerState<ASpellPlayerState>())
    {
        if (UAbilitySystemComponent* ASC = SpellPS->GetAbilitySystemComponent())
        {
            // Remove remaining tags
            FGameplayTagContainer TagsToRemove;
            TagsToRemove.AddTag(SpellGameplayTags::State_Dead);
            ASC->RemoveActiveEffectsWithGrantedTags(TagsToRemove);

            // Reset attributes
            SpellPS->GetAttributeSet()->SetHealth(SpellPS->GetAttributeSet()->GetMaxHealth());
            SpellPS->GetAttributeSet()->SetMana(SpellPS->GetAttributeSet()->GetMaxMana());
        }
    }

    if (APawn* OldPawn = Controller->GetPawn())
    {
        OldPawn->Destroy();
    }

    // Built in method for respawning
    RestartPlayer(Controller);
}

void ASpellGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // Give each player a unique TeamID (free for all ruleset)
    // Currently used by Capture Point logic
    if (ASpellPlayerState* SpellPS = NewPlayer->GetPlayerState<ASpellPlayerState>())
    {
        int32 UniqueID = TeamIDCount++;
        SpellPS->TeamID = UniqueID;
    }
}

void ASpellGameMode::HandleMatchHasEnded()
{
    Super::HandleMatchHasEnded();

    // Show end screen to host as well
    if (ASpellGameState* SpellGS = GetGameState<ASpellGameState>())
    {
        SpellGS->HandleShowEndScreen();
    }
}

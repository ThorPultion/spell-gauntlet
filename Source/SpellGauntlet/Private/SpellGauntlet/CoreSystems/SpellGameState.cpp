// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellGauntlet/Coresystems/SpellGameState.h"
#include "SpellGauntlet/Player/SpellPlayerState.h"
#include "GameFramework/GameMode.h"
#include "SpellGauntlet/Player/SpellPlayerController.h"

int32 ASpellGameState::GetTotalKillsForTeam(int32 TargetTeamID)
{
    int32 Total = 0;
    for (APlayerState* PS : PlayerArray)
    {
        ASpellPlayerState* SpellPS = Cast<ASpellPlayerState>(PS);
        if (SpellPS && SpellPS->TeamID == TargetTeamID)
        {
            Total += SpellPS->KillCount;
        }
    }
    return Total;
}

// Overriding AddPlayerState for UI update purposes
void ASpellGameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);

    if (PlayerState)
    {
        // Broadcasting to blueprints for when a new player has joined
        OnPlayerJoined.Broadcast(PlayerState);
    }
}

// Blueprint function for getting top players
TArray<APlayerState*> ASpellGameState::GetTopScorers()
{
    TArray<APlayerState*> SortedPlayers = PlayerArray;

    SortedPlayers.Sort([](const APlayerState& A, const APlayerState& B) {
        return A.GetScore() > B.GetScore();
        });

    int32 TopCount = FMath::Min(LeaderboardLength, SortedPlayers.Num());
    SortedPlayers.SetNum(TopCount);

    return SortedPlayers;
}

void ASpellGameState::OnRep_MatchState()
{
    Super::OnRep_MatchState();

    if (MatchState == MatchState::WaitingPostMatch)
    {
        HandleShowEndScreen();
    }
}

void ASpellGameState::HandleShowEndScreen()
{
    APlayerState* WinningPlayer = GetTopScorers()[0];

    OnMatchEnded.Broadcast(WinningPlayer);
}

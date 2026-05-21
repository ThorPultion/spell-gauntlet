// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpellGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, APlayerState*, NewPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchEnded, const APlayerState*, WinningPlayer);

UCLASS()
class SPELLGAUNTLET_API ASpellGameState : public AGameState
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    int32 GetTotalKillsForTeam(int32 TargetTeamID);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPlayerJoined OnPlayerJoined;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMatchEnded OnMatchEnded;

    // We override this built-in engine function to catch when a new player joins
    virtual void AddPlayerState(APlayerState* PlayerState) override;

    UFUNCTION(BlueprintPure, Category = "Arena")
    TArray<APlayerState*> GetTopScorers();

    /** How many players fit on leaderboard UI */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaderboard")
    int32 LeaderboardLength = 3;

    virtual void OnRep_MatchState() override;

    void HandleShowEndScreen();
};

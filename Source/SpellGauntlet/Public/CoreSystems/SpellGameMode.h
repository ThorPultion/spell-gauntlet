// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SpellGameMode.generated.h"

class ASpellPlayerState;
/**
 * 
 */
UCLASS()
class SPELLGAUNTLET_API ASpellGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerScore(ASpellPlayerState* SpellPS, int32 ScoreIncrement);

	/** Function to handle the scoring logic and respawn */
	UFUNCTION(BlueprintCallable, Category = "Player Death")
	void PlayerKilled(AController* Killer, AController* Victim);

	UFUNCTION(BlueprintCallable, Category = "Player Death")
	void RequestRespawn(AController* Controller);

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleMatchHasEnded() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Rules")
	float RespawnDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Rules")
	int32 ScoreToWin = 20;

	UPROPERTY(EditAnywhere, Category = "Rules")
	int32 ScoreOnPlayerKill = 1;

	/** After game is over, restart after this duration */
	UPROPERTY(EditAnywhere, Category = "Rules")
	float GameRestartTimer = 3.0f;

	UPROPERTY()
	int32 TeamIDCount = 0;

	UPROPERTY()
	TArray<AActor*> SpawnPoints;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	// Helper to actually perform the spawn
	void RespawnPlayer(AController* Controller);
private:

	void CheckWinCondition(ASpellPlayerState* SpellPS);
};

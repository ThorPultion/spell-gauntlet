// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SpellPlayerController.h"
#include "CoreSystems/SpellGameState.h"

void ASpellPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	BP_OnAcknowledgePossession();
}

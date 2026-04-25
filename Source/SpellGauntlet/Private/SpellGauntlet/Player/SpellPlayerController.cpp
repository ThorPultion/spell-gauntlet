// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellGauntlet/Player/SpellPlayerController.h"
#include "SpellGauntlet/CoreSystems/SpellGameState.h"

void ASpellPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	BP_OnAcknowledgePossession();
}

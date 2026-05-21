// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpellPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SPELLGAUNTLET_API ASpellPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void AcknowledgePossession(APawn* P) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void BP_OnAcknowledgePossession();
};

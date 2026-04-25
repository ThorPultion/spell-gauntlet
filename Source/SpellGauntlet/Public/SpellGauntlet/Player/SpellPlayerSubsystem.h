// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SpellPlayerSubsystem.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSystemsReady);

UCLASS()
class SPELLGAUNTLET_API USpellPlayerSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    /** Broadcasted when player character has initiated player controller and player controller is done with creating UI.
    Used for different initializations */
    UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
    FOnPlayerSystemsReady OnPlayerSystemsReady;
};
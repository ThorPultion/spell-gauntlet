// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellGauntlet/Gameplay/Abilities/GA_CastBase.h"
#include "GA_CastProjectile.generated.h"

/**
 * 
 */
class UAnimMontage;

UCLASS()
class SPELLGAUNTLET_API UGA_CastProjectile : public UGA_CastBase
{
	GENERATED_BODY()

protected:

    /** The projectile to spawn */
    UPROPERTY(EditDefaultsOnly, Category = "SpellInfo")
    TSubclassOf<AActor> ProjectileBP;

    UPROPERTY(EditAnywhere, Category = "State")
    FGameplayTag CastingTag;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    float UpwardsSpawnOffset = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    float ForwardsSpawnOffset = 100.0f;

    // This is hooked in base class to when "cast" is over
    virtual void OnMontageEventReceived(FGameplayEventData Payload) override;
};

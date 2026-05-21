// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpellDefinition.generated.h"

/**
 * 
 */
class UGameplayAbility;

UCLASS(BlueprintType)
class SPELLGAUNTLET_API USpellDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** The display name of the spell */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
    FText SpellName;

    /** SOFT reference to the Ability Class (The Memory Saver!) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
    TSoftClassPtr<UGameplayAbility> SpellAbilityClass;

    /** UI Icon for the spell */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
    TSoftObjectPtr<UTexture2D> SpellIcon;

    /** Unique ID for the Asset Manager */
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
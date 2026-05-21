// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/AbilitySystem/SpellDefinition.h"

FPrimaryAssetId USpellDefinition::GetPrimaryAssetId() const
{
    // "Spell" matches the type you'll register in Project Settings
    return FPrimaryAssetId("Spell", GetFName());
}
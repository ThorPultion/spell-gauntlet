// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "GameplayTagContainer.h"
#include "BaseDamageType.generated.h"

/**
 * 
 */
UCLASS()
class SPELLGAUNTLET_API UBaseDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
    /** Define damage type by tag */ 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    FGameplayTagContainer DamageTags;
};

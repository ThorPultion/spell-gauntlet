// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SpellGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class SPELLGAUNTLET_API USpellGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	/** Should the ability show in abilitybar on HUD? */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	bool bShowInAbilitiesBar = false;
	
	/** Should the ability activation be height controlled? */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	bool bHeightControlledActivation = false;

	/** Height required for activation */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeightControlledActivation", meta = (EditCondition = "bHeightControlledActivation", EditConditionHides))
	float DistanceRequired = 300;
};

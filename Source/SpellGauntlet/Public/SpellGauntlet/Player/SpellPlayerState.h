// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SpellGauntlet/Gameplay/AbilitySystem/SpellAbilitySystemComponent.h"
#include "SpellPlayerState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, float, NewScore);

UCLASS()
class SPELLGAUNTLET_API ASpellPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 DeathCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 KillCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 TeamID = 0;

	//** Delegate for UI */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnScoreChanged OnScoreChanged;

public:
	ASpellPlayerState();

	// Required by IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Accessor for the AttributeSet */
	class USpellAttributeSet* GetAttributeSet() const { return SpellAttributeSet; }

	UFUNCTION(BlueprintCallable)
	void UpdateScore(int32 NewScore);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Abilities")
	TObjectPtr<USpellAbilitySystemComponent> SpellAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
	TObjectPtr<USpellAttributeSet> SpellAttributeSet;

	// We override the built-in function that triggers when Score replicates
	virtual void OnRep_Score() override;
};

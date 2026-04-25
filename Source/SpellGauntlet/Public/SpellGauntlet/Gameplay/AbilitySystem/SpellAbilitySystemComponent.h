// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SpellAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPELLGAUNTLET_API USpellAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	USpellAbilitySystemComponent();

	// Making sure ability changes are synced by keeping track of an increment with onrep
	UPROPERTY(ReplicatedUsing = OnRep_AbilitySyncCounter)
	int32 AbilitySyncCounter = 0;

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void SendAbilitiesChangedEvent();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "AbilitySystem")
	void Server_GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "AbilitySystem")
	void Server_RemoveAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove);

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_AbilitySyncCounter();

	TArray<FGameplayAbilitySpecHandle> GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant);

	void RemoveAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove);

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	FGameplayTag AbilitiesChangedTag;
};
